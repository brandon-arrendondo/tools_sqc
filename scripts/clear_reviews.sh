#!/bin/bash
# Clear all reviews from STAGED proposal frontmatter

STAGED_DIR="AGENTS/PROPOSALS/STAGED"

for file in "$STAGED_DIR"/P*-implementation.md; do
    filename=$(basename "$file")

    # Use awk to process the file:
    # - In frontmatter (between first and second ---), replace reviews block with empty array
    # - Preserve everything else
    awk '
    BEGIN { in_frontmatter = 0; in_reviews = 0; frontmatter_count = 0 }
    /^---$/ {
        frontmatter_count++
        if (frontmatter_count == 1) {
            in_frontmatter = 1
            print
            next
        }
        if (frontmatter_count == 2) {
            in_frontmatter = 0
            in_reviews = 0
            print
            next
        }
    }
    in_frontmatter && /^reviews:/ {
        print "reviews: []"
        in_reviews = 1
        next
    }
    in_frontmatter && in_reviews && /^  / {
        # Skip indented content under reviews
        next
    }
    in_frontmatter && in_reviews && /^[^ ]/ {
        # No longer in reviews block
        in_reviews = 0
    }
    { print }
    ' "$file" > "$file.tmp" && mv "$file.tmp" "$file"

    echo "CLEARED: $filename"
done

echo ""
echo "Reviews cleared from all proposals!"
