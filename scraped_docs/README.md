# Scraped CERT C Wiki Documentation

This directory contains documentation scraped directly from the SEI CERT C Coding Standard Confluence wiki.

## Contents

- `rules/cert-c/` - All CERT C items (both rules and recommendations) organized by category
  - Each rule/recommendation has its own nested directory
  - Example: `rules/cert-c/ARR/ARR30/ARR30-C.yaml` (rule metadata)
  - Test cases: `rules/cert-c/ARR/ARR30/tests/fail/` and `tests/pass/`
  - The `-C` suffix in filenames indicates CERT C standard

## Source

All content scraped from: https://wiki.sei.cmu.edu/confluence/display/c

## Generation

Generated using `scripts/scrape_cert_wiki.py` with:
- Conservative 3-second delay between requests (no robots.txt found)
- Educational purpose scraping
- Respectful of server resources

## Rate Limiting

Since no robots.txt was found at the wiki, we use a conservative default:
- **Default delay:** 3.0 seconds between requests
- **Recommended:** Increase delay if scraping large numbers of rules
- **Usage:** `python3 scripts/scrape_cert_wiki.py --delay 5.0`

## Freshness

This is a snapshot of the wiki at the time of scraping. For the most current information, always refer to the official wiki.

## Structure

```
scraped_docs/
├── README.md (this file)
└── rules/
    └── cert-c/
        ├── ARR/
        │   ├── ARR00/
        │   │   ├── ARR00-C.yaml      (recommendation metadata)
        │   │   └── tests/             (if examples exist)
        │   │       ├── fail/
        │   │       │   └── *.c
        │   │       └── pass/
        │   │           └── *.c
        │   ├── ARR30/
        │   │   ├── ARR30-C.yaml      (rule metadata)
        │   │   └── tests/
        │   │       ├── fail/
        │   │       │   ├── forming_out_of_bounds_pointer.c
        │   │       │   ├── dereferencing_past_end.c
        │   │       │   └── ...
        │   │       └── pass/
        │   │           ├── forming_out_of_bounds_pointer.c
        │   │           ├── dereferencing_past_end.c
        │   │           └── ...
        │   └── ...
        ├── MEM/
        │   ├── MEM30/
        │   │   ├── MEM30-C.yaml
        │   │   └── tests/
        │   └── ...
        └── ...
```

## Usage Examples

### Scrape all rules AND recommendations (will take a while with 3s delay)
```bash
python3 scripts/scrape_cert_wiki.py --type all
```

### Scrape only rules
```bash
python3 scripts/scrape_cert_wiki.py --type rule
```

### Scrape only recommendations
```bash
python3 scripts/scrape_cert_wiki.py --type rec
```

### Scrape specific categories only
```bash
python3 scripts/scrape_cert_wiki.py --categories ARR MEM STR --type all
```

### Use a longer delay (recommended for full scrapes)
```bash
python3 scripts/scrape_cert_wiki.py --delay 5.0
```

### Custom output location
```bash
python3 scripts/scrape_cert_wiki.py --output my_custom_dir/
```

## Note

This directory is excluded from git by default to avoid committing large amounts of scraped content. Run the scraper locally as needed.
