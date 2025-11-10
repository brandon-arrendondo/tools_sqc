#!/usr/bin/env python3
"""
Scrape SEI CERT C Coding Standard from Confluence Wiki.

This script:
1. Fetches all rule and recommendation categories from the main wiki page
2. Extracts individual items (rules/recommendations) from each category
3. Parses each page for content and metadata
4. Generates YAML metadata only
5. Implements rate limiting to be respectful of the wiki

Usage:
    python3 scripts/scrape_cert_wiki.py [--delay SECONDS] [--output DIR] [--type rule|rec|all]
"""

import re
import os
import sys
import time
import yaml
import json
import argparse
import requests
from pathlib import Path
from typing import Dict, List, Tuple, Optional
from dataclasses import dataclass, asdict
from urllib.parse import urljoin, quote
from bs4 import BeautifulSoup
from datetime import datetime

# Configuration
BASE_URL = "https://wiki.sei.cmu.edu"
WIKI_BASE = f"{BASE_URL}/confluence/display/c"
DEFAULT_DELAY = 3.0  # Seconds between requests (conservative - no robots.txt found)
USER_AGENT = "CERT-C-Scraper/1.0 (Educational Purpose)"
SCHEMA_VERSION = "1.0.0"  # Version of the YAML structure format

# Output configuration
BASE_OUTPUT_DIR = "scraped_docs"
RULES_SUBDIR = "rules"
CERT_C_SUBDIR = "cert-c"

# Computed paths
OUTPUT_DIR = os.path.join(BASE_OUTPUT_DIR, RULES_SUBDIR, CERT_C_SUBDIR)

# Category mapping - will be populated dynamically from wiki
# Format: {"CAT": ("number", "Category Name")}
CATEGORIES = {}


@dataclass
class ItemMetadata:
    """Structured metadata for a rule or recommendation from wiki"""
    id: str
    item_type: str  # "rule" or "recommendation"
    category: str
    number: int
    title: str

    # Risk assessment
    severity: Optional[str] = None
    likelihood: Optional[str] = None
    priority: Optional[str] = None
    level: Optional[str] = None

    # References
    wiki_url: str = ""
    cert_version: Optional[str] = None  # CERT C standard version from wiki
    last_modified: Optional[str] = None  # Last modified date from wiki
    cwe: List[str] = None
    related_rules: List[str] = None
    related_recommendations: List[str] = None

    # Content
    description: str = ""

    # Implementation status (from existing code)
    implemented: bool = False
    documented: bool = False
    tested: bool = False

    def __post_init__(self):
        if self.cwe is None:
            self.cwe = []
        if self.related_rules is None:
            self.related_rules = []
        if self.related_recommendations is None:
            self.related_recommendations = []


class WikiScraper:
    """Scraper for CERT C Confluence wiki"""

    def __init__(self, delay: float = DEFAULT_DELAY):
        self.session = requests.Session()
        self.session.headers.update({'User-Agent': USER_AGENT})
        self.delay = delay
        self.last_request_time = 0

    def _rate_limit(self):
        """Implement rate limiting between requests"""
        elapsed = time.time() - self.last_request_time
        if elapsed < self.delay:
            time.sleep(self.delay - elapsed)
        self.last_request_time = time.time()

    def fetch_page(self, url: str) -> Optional[BeautifulSoup]:
        """Fetch and parse a wiki page with rate limiting"""
        self._rate_limit()

        try:
            print(f"  Fetching: {url}")
            response = self.session.get(url, timeout=30)
            response.raise_for_status()
            return BeautifulSoup(response.content, 'html.parser')
        except requests.RequestException as e:
            print(f"  ✗ Error fetching {url}: {e}")
            return None

    def discover_categories(self) -> Dict[str, Tuple[str, str]]:
        """
        Dynamically discover all categories from the main wiki page.

        Returns: Dict mapping category code to (number, name)
        Example: {"ARR": ("06", "Arrays"), "MSC": ("48", "Miscellaneous")}
        """
        print("Discovering categories from main wiki page...")
        soup = self.fetch_page(WIKI_BASE)
        if not soup:
            print("  ✗ Failed to fetch main wiki page, using empty categories")
            return {}

        categories = {}

        # Find all links that match "Rule XX" or "Rec. XX" patterns
        # Pattern: "Rule 01. Preprocessor (PRE)" or "Rec. 01. Preprocessor (PRE)"
        for link in soup.find_all('a', href=True):
            link_text = link.get_text(strip=True)

            # Match patterns like:
            # "Rule 06. Arrays (ARR)"
            # "Rec. 48. Miscellaneous (MSC)"
            match = re.match(r'(?:Rule|Rec\.)\s+(\d+)\.\s+([^(]+)\s+\((\w+)\)', link_text)
            if match:
                cat_num, cat_name, cat_code = match.groups()
                cat_name = cat_name.strip()

                # Only add if we haven't seen this category code yet
                # (rules and recommendations might both list it)
                if cat_code not in categories:
                    categories[cat_code] = (cat_num, cat_name)
                    print(f"  Found category: {cat_code} = {cat_num}. {cat_name}")

        print(f"✓ Discovered {len(categories)} categories")
        return categories

    def get_category_items(self, category_code: str, item_type: str) -> List[Tuple[str, str, str]]:
        """
        Get all items (rules or recommendations) from a category page.

        Args:
            category_code: Category code (e.g., "ARR", "MEM")
            item_type: Either "rule" or "recommendation"

        Returns: List of (item_id, item_title, item_url) tuples
        """
        cat_num, cat_name = CATEGORIES[category_code]

        # Build URL based on type
        if item_type == "rule":
            prefix = "Rule"
        else:
            prefix = "Rec."

        # URL format: Rule+06.+Arrays+(ARR)
        cat_url = f"{WIKI_BASE}/{prefix}+{cat_num}.+{cat_name.replace(' ', '+')}+({category_code})"

        soup = self.fetch_page(cat_url)
        if not soup:
            return []

        items = []
        # Find all links that match item pattern: XXX##-C
        item_pattern = re.compile(rf'^{category_code}\d{{2}}-C')

        for link in soup.find_all('a', href=True):
            text = link.get_text(strip=True)
            if item_pattern.match(text):
                # Extract item ID from link text
                match = re.match(rf'({category_code}\d{{2}}-C)', text)
                if match:
                    item_id = match.group(1)
                    # Get the full title (may be in following text)
                    title = text[len(item_id):].strip('. ')

                    # Construct full URL
                    href = link['href']
                    if href.startswith('/'):
                        item_url = urljoin(BASE_URL, href)
                    else:
                        item_url = href

                    items.append((item_id, title, item_url))
                    print(f"    Found: {item_id} - {title}")

        return items

    def parse_item_page(self, item_id: str, item_url: str, item_type: str) -> Optional[Tuple[ItemMetadata, List[str], List[str]]]:
        """
        Parse an individual rule or recommendation page for all content.

        Returns: (ItemMetadata, non_compliant_examples, compliant_examples) or None on error
        """
        soup = self.fetch_page(item_url)
        if not soup:
            return None

        # Extract category and number from item ID
        match = re.match(r'^([A-Z]{3})(\d{2})-C$', item_id)
        if not match:
            return None
        category, number = match.groups()

        # Initialize metadata
        item = ItemMetadata(
            id=item_id,
            item_type=item_type,
            category=category,
            number=int(number),
            title="",
            wiki_url=item_url
        )

        # Extract title from page
        title_elem = soup.find('h1', id='title-text')
        if title_elem:
            title_text = title_elem.get_text(strip=True)
            # Remove item ID prefix if present
            item.title = re.sub(rf'^{item_id}\.?\s*', '', title_text)

        # Extract last modified date from page footer/metadata
        # Pattern: "last modified by [User] on [Month DD, YYYY]"
        full_text = soup.get_text()
        modified_match = re.search(r'last\s+modified\s+by\s+[^\n]+\s+on\s+([A-Z][a-z]+\s+\d{1,2},\s+\d{4})', full_text, re.IGNORECASE)
        if modified_match:
            item.last_modified = modified_match.group(1)
        else:
            # Try alternate format in metadata banner
            page_meta = soup.find('div', id='page-metadata-banner') or soup.find('div', class_='page-metadata')
            if page_meta:
                meta_text = page_meta.get_text()
                modified_match = re.search(r'(?:Last\s+Modified|Updated):\s*([A-Za-z]+\s+\d{1,2},\s+\d{4})', meta_text, re.IGNORECASE)
                if modified_match:
                    item.last_modified = modified_match.group(1)

        # cert_version will be added in the future once source is identified
        # For now, using baseline reference from main wiki page
        item.cert_version = "2016 Edition (Wiki)"

        # Extract main content
        content = soup.find('div', id='main-content')
        if not content:
            return item

        # Extract description (first paragraph or section before first heading)
        desc_parts = []
        for elem in content.find_all(['p', 'div'], recursive=False):
            text = elem.get_text(strip=True)
            if text and not text.startswith('Rule') and not text.startswith('Rec'):
                desc_parts.append(text)
                if len(desc_parts) >= 3:  # Get first few paragraphs
                    break
        item.description = '\n\n'.join(desc_parts)

        # Extract risk assessment table
        for table in content.find_all('table'):
            headers = [th.get_text(strip=True).lower() for th in table.find_all('th')]
            if 'severity' in headers or 'likelihood' in headers:
                rows = table.find_all('tr')
                if len(rows) > 1:
                    cells = rows[1].find_all(['td', 'th'])
                    for i, header in enumerate(headers):
                        if i < len(cells):
                            value = cells[i].get_text(strip=True)
                            if header == 'severity':
                                item.severity = value
                            elif header == 'likelihood':
                                item.likelihood = value
                            elif header == 'priority':
                                item.priority = value
                            elif header == 'level':
                                item.level = value

        # Extract CWE references
        cwe_pattern = re.compile(r'CWE-(\d+)')
        for match in cwe_pattern.finditer(str(content)):
            cwe_id = f"CWE-{match.group(1)}"
            if cwe_id not in item.cwe:
                item.cwe.append(cwe_id)

        # Extract related rules and recommendations
        rule_pattern = re.compile(r'\b([A-Z]{3}\d{2}-C)\b')

        # Look for "Related" section (check multiple possible headings)
        for heading in content.find_all(['h2', 'h3', 'h4']):
            heading_text = heading.get_text(strip=True)
            if 'related' in heading_text.lower():
                # Get the next section after this heading
                next_section = heading.find_next_sibling()
                if next_section:
                    section_text = next_section.get_text()

                    for match in rule_pattern.finditer(section_text):
                        related_id = match.group(1)
                        if related_id == item_id:
                            continue

                        # Try to determine if it's a rule or recommendation
                        # Rules typically have higher numbers (30+), recommendations lower (00-29)
                        related_num = int(related_id[3:5])

                        # Also check context around the match
                        start = max(0, match.start() - 50)
                        end = min(len(section_text), match.end() + 50)
                        context = section_text[start:end].lower()

                        if 'recommendation' in context or related_num < 30:
                            if related_id not in item.related_recommendations:
                                item.related_recommendations.append(related_id)
                        else:
                            if related_id not in item.related_rules:
                                item.related_rules.append(related_id)
                break  # Only process first "Related" section

        # Check implementation status from existing code
        file_name = item_id.lower().replace('-c', '_c')
        impl_file = Path(f"src/rules/cert_c/{file_name}.rs")
        test_file = Path(f"src/rules/cert_c/tests/{file_name}.rs")
        doc_file1 = Path(f"docs/cert-c-rules/{item_id}.md")
        doc_file2 = Path(f"docs/cert-c-rules/{item_id.lower()}.md")

        item.implemented = impl_file.exists()
        item.tested = test_file.exists()
        item.documented = doc_file1.exists() or doc_file2.exists()

        # Extract code examples
        non_compliant, compliant = extract_code_examples(content, item_id)

        return (item, non_compliant, compliant)


def sanitize_code(code: str) -> str:
    """
    Clean invisible/non-printable characters from code.
    Removes non-breaking spaces, zero-width spaces, etc.
    """
    # Replace non-breaking space (U+00A0) with regular space
    code = code.replace('\u00a0', ' ')
    # Replace zero-width space (U+200B)
    code = code.replace('\u200b', '')
    # Replace zero-width non-joiner (U+200C)
    code = code.replace('\u200c', '')
    # Replace zero-width joiner (U+200D)
    code = code.replace('\u200d', '')
    # Replace other common invisible characters
    code = code.replace('\ufeff', '')  # Zero-width no-break space (BOM)
    return code


def extract_code_examples(content, item_id: str) -> Tuple[List[Tuple[str, str]], List[Tuple[str, str]]]:
    """
    Extract compliant and non-compliant code examples from page content.

    Returns: (non_compliant_examples, compliant_examples)
    Each tuple is (example_name, code)
    """
    non_compliant = []
    compliant = []

    # Find all headings and their code blocks
    for heading in content.find_all(['h2', 'h3', 'h4', 'h5']):
        heading_text = heading.get_text(strip=True)
        heading_lower = heading_text.lower()

        # Create a clean filename from heading (remove "Noncompliant Code Example", etc.)
        clean_name = heading_text
        for remove_phrase in ['Noncompliant Code Example', 'Compliant Solution', 'Non-Compliant Code Example']:
            clean_name = re.sub(re.escape(remove_phrase), '', clean_name, flags=re.IGNORECASE)
        clean_name = clean_name.strip(' :-')

        # If no descriptive name, use generic counter
        if not clean_name or len(clean_name) < 3:
            clean_name = None

        # Check if this is a non-compliant example
        if 'noncompliant' in heading_lower or 'non-compliant' in heading_lower:
            # Find all code blocks after this heading until next heading
            current = heading.find_next_sibling()
            code_count = 0
            while current and current.name not in ['h2', 'h3', 'h4', 'h5']:
                if current.name == 'pre' or (current.name == 'div' and 'code' in current.get('class', [])):
                    code = current.get_text()
                    code = sanitize_code(code)  # Clean invisible characters
                    if code.strip():
                        code_count += 1
                        if clean_name:
                            example_name = sanitize_filename(clean_name)
                        else:
                            example_name = f"noncompliant_{len(non_compliant) + 1}"

                        # If multiple code blocks under same heading, add suffix
                        if code_count > 1:
                            example_name = f"{example_name}_{code_count}"

                        non_compliant.append((example_name, code.strip()))
                current = current.find_next_sibling()

        # Check if this is a compliant example
        elif 'compliant' in heading_lower and 'non' not in heading_lower:
            # Find all code blocks after this heading until next heading
            current = heading.find_next_sibling()
            code_count = 0
            while current and current.name not in ['h2', 'h3', 'h4', 'h5']:
                if current.name == 'pre' or (current.name == 'div' and 'code' in current.get('class', [])):
                    code = current.get_text()
                    code = sanitize_code(code)  # Clean invisible characters
                    if code.strip():
                        code_count += 1
                        if clean_name:
                            example_name = sanitize_filename(clean_name)
                        else:
                            example_name = f"compliant_{len(compliant) + 1}"

                        # If multiple code blocks under same heading, add suffix
                        if code_count > 1:
                            example_name = f"{example_name}_{code_count}"

                        compliant.append((example_name, code.strip()))
                current = current.find_next_sibling()

    return non_compliant, compliant


def sanitize_filename(name: str) -> str:
    """Convert heading text to valid filename"""
    # Replace special characters with underscores
    name = re.sub(r'[^\w\s-]', '', name)
    name = re.sub(r'[-\s]+', '_', name)
    name = name.strip('_')
    name = name.lower()
    # Limit length
    if len(name) > 60:
        name = name[:60]
    return name


def save_code_examples(item_id: str, category: str, non_compliant: List[Tuple[str, str]], compliant: List[Tuple[str, str]], output_dir: str):
    """Save code examples as test files"""
    if not non_compliant and not compliant:
        return

    # Extract rule number without -C suffix for directory name
    # ARR30-C -> ARR30
    rule_dir_name = item_id.replace('-C', '')

    # Create nested test directories: ARR/ARR30/tests/fail and ARR/ARR30/tests/pass
    tests_dir = Path(output_dir) / category / rule_dir_name / "tests"
    fail_dir = tests_dir / "fail"
    pass_dir = tests_dir / "pass"

    fail_dir.mkdir(parents=True, exist_ok=True)
    pass_dir.mkdir(parents=True, exist_ok=True)

    # Save non-compliant examples
    for example_name, code in non_compliant:
        filename = f"{example_name}.c"
        filepath = fail_dir / filename
        filepath.write_text(code)
        print(f"    ✓ Saved non-compliant example: {filepath}")

    # Save compliant examples
    for example_name, code in compliant:
        filename = f"{example_name}.c"
        filepath = pass_dir / filename
        filepath.write_text(code)
        print(f"    ✓ Saved compliant example: {filepath}")


def generate_yaml_metadata(item: ItemMetadata, output_path: Path):
    """Generate YAML metadata file for a rule or recommendation"""

    # Extract rule number without -C suffix for directory name
    rule_dir_name = item.id.replace('-C', '')

    yaml_content = {
        "id": item.id,
        "type": item.item_type,  # "rule" or "recommendation"
        "category": item.category,
        "number": item.number,
        "metadata": {
            "title": item.title,
            "description": item.description,  # Full description, no truncation
            "severity": item.severity or "Unknown",
            "likelihood": item.likelihood or "Unknown",
            "priority": item.priority or "Unknown",
            "level": item.level or "Unknown",
            "cert_version": item.cert_version or "Unknown",
            "last_modified": item.last_modified or "Unknown"
        },
        "implementation": {
            "status": "implemented" if item.implemented else "not_implemented",
            "file": os.path.join("src", RULES_SUBDIR, CERT_C_SUBDIR, item.category, rule_dir_name, f"{item.id.lower().replace('-c', '_c')}.rs") if item.implemented else None
        },
        "testing": {
            "status": "tested" if item.tested else "not_tested",
            "test_file": os.path.join("src", RULES_SUBDIR, CERT_C_SUBDIR, item.category, rule_dir_name, f"test_{item.id.lower().replace('-c', '_c')}.rs") if item.tested else None,
            "external_tests": f"~/tools_sqc_testcases/{item.id}/"
        },
        "references": {
            "wiki": item.wiki_url,
            "cwe": item.cwe,
            "related_rules": item.related_rules,
            "related_recommendations": item.related_recommendations
        }
    }

    with open(output_path, 'w') as f:
        yaml.dump(yaml_content, f, default_flow_style=False, sort_keys=False, allow_unicode=True)


def main():
    """Main execution function"""
    parser = argparse.ArgumentParser(description='Scrape CERT C wiki and generate YAML metadata')
    parser.add_argument('--delay', type=float, default=DEFAULT_DELAY,
                        help=f'Delay between requests in seconds (default: {DEFAULT_DELAY})')
    parser.add_argument('--output', type=str, default=BASE_OUTPUT_DIR,
                        help=f'Output base directory (default: {BASE_OUTPUT_DIR})')
    parser.add_argument('--categories', type=str, nargs='+',
                        help='Specific categories to scrape (e.g., ARR MEM), default: all')
    parser.add_argument('--type', choices=['rule', 'rec', 'all'], default='all',
                        help='Scrape rules, recommendations, or both (default: all)')
    args = parser.parse_args()

    print("=" * 60)
    print("CERT C Wiki Scraper - YAML Only")
    print("=" * 60)
    print(f"Rate limit: {args.delay}s between requests")
    print(f"  (Note: No robots.txt found - using conservative default)")
    print(f"Output directory: {args.output}")
    print(f"Scraping: {args.type}")
    print()

    # Create scraper
    scraper = WikiScraper(delay=args.delay)

    # Discover categories dynamically from wiki
    global CATEGORIES
    CATEGORIES = scraper.discover_categories()
    print()

    if not CATEGORIES:
        print("✗ No categories discovered from wiki. Exiting.")
        return 1

    # Determine which categories to scrape
    categories_to_scrape = args.categories if args.categories else CATEGORIES.keys()

    # Determine what types to scrape
    types_to_scrape = []
    if args.type in ['rule', 'all']:
        types_to_scrape.append('rule')
    if args.type in ['rec', 'all']:
        types_to_scrape.append('recommendation')

    # Collect all items
    all_items = []

    for item_type in types_to_scrape:
        print(f"\n{'='*60}")
        print(f"Collecting {item_type.upper()}S from category pages...")
        print(f"{'='*60}")

        for category_code in categories_to_scrape:
            if category_code not in CATEGORIES:
                print(f"  ✗ Unknown category: {category_code}")
                continue

            print(f"\n[{category_code}] Fetching {item_type} category page...")
            items = scraper.get_category_items(category_code, item_type)
            print(f"  ✓ Found {len(items)} {item_type}s")
            all_items.extend([(item_type, category_code, item_id, title, url)
                            for item_id, title, url in items])

    print(f"\n✓ Total items found: {len(all_items)}")
    print()

    # Parse each item
    print("=" * 60)
    print("Parsing individual pages and generating YAML...")
    print("=" * 60)
    parsed_items = []

    for i, (item_type, category, item_id, title, url) in enumerate(all_items, 1):
        print(f"[{i}/{len(all_items)}] Parsing {item_type} {item_id}...")

        result = scraper.parse_item_page(item_id, url, item_type)
        if result:
            item, non_compliant, compliant = result

            # If title wasn't extracted from page, use the one from category page
            if not item.title and title:
                item.title = title

            parsed_items.append(item)

            # Create directory structure organized by category with nested rule directories
            # Structure: scraped_docs/rules/cert-c/ARR/ARR30/ARR30-C.yaml
            base_output_dir = os.path.join(args.output, RULES_SUBDIR, CERT_C_SUBDIR)

            # Extract rule number without -C suffix for directory name
            # ARR30-C -> ARR30
            rule_dir_name = item.id.replace('-C', '')

            rule_dir = Path(base_output_dir) / item.category / rule_dir_name
            rule_dir.mkdir(parents=True, exist_ok=True)

            # Generate YAML - filename keeps -C suffix for CERT C identification
            yaml_filename = item.id + '.yaml'
            yaml_path = rule_dir / yaml_filename
            generate_yaml_metadata(item, yaml_path)
            print(f"  ✓ Generated YAML: {yaml_path}")

            # Save code examples as test files
            if non_compliant or compliant:
                save_code_examples(item.id, item.category, non_compliant, compliant, base_output_dir)
        else:
            print(f"  ✗ Failed to parse {item_id}")

    print()
    print("=" * 60)
    print("✅ SCRAPING COMPLETE!")
    print("=" * 60)
    print(f"Scraped {len(parsed_items)} items")

    # Count by type
    rules_count = sum(1 for item in parsed_items if item.item_type == "rule")
    rec_count = sum(1 for item in parsed_items if item.item_type == "recommendation")
    print(f"  - Rules: {rules_count}")
    print(f"  - Recommendations: {rec_count}")

    print(f"Output directory: {args.output}")
    print()
    print("Next steps:")
    print("1. Review generated YAML files")
    print("2. Validate content against wiki")
    print("3. Use YAML metadata for tooling/documentation generation")


if __name__ == "__main__":
    main()
