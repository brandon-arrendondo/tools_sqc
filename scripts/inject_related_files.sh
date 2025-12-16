#!/bin/bash
# Inject related_files into all proposal frontmatter in STAGED directory

STAGED_DIR="AGENTS/PROPOSALS/STAGED"

for file in "$STAGED_DIR"/P*-implementation.md; do
    filename=$(basename "$file")

    # Extract rule ID: P1-API01-C-implementation.md -> API01-C
    rule_id=$(echo "$filename" | sed 's/^P[0-9]*-//' | sed 's/-implementation\.md$//')

    # Extract category: API01-C -> API, MEM36-C -> MEM, ARR30-C -> ARR
    category=$(echo "$rule_id" | sed 's/[0-9].*$//')

    # Check if related_files already exists in the file
    if grep -q "^related_files:" "$file"; then
        echo "SKIP: $filename (already has related_files)"
        continue
    fi

    # Create the related_files block
    related_block="related_files:
  - src/rules/cert_c/${category}/${rule_id}/
  - src/rules/cert_c/mod.rs
  - src/utility/cert_c/"

    # Insert before the closing --- of frontmatter
    # The frontmatter structure is:
    # ---
    # reviews:
    #   ...
    # ---
    # We need to insert before the second ---

    # Use awk to insert the block before the second ---
    awk -v block="$related_block" '
    BEGIN { count = 0 }
    /^---$/ {
        count++
        if (count == 2) {
            print block
        }
    }
    { print }
    ' "$file" > "$file.tmp" && mv "$file.tmp" "$file"

    echo "DONE: $filename -> ${rule_id} (${category})"
done

echo ""
echo "Injection complete!"
