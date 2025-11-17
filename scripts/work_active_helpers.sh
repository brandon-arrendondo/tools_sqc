#!/bin/bash
# Unified helper scripts for work-active workflow and Claude modes
# Consolidates: claude_mode_impl_rule.sh, claude_mode_reset.sh, etc.
# Avoids Claude Code escaping issues with inline $() constructs

set -e

ACTIVE_DIR="AGENTS/PROPOSALS/ACTIVE"

case "$1" in
    "count-subdirs")
        # Count proposals in each subdirectory
        for dir in "$ACTIVE_DIR"/*/; do
            if [ -d "$dir" ]; then
                name=$(basename "$dir")
                count=$(ls "$dir"/*.md 2>/dev/null | wc -l)
                echo "$name: $count proposals"
            fi
        done
        ;;

    "list-subdirs")
        # List subdirectory names only
        for dir in "$ACTIVE_DIR"/*/; do
            if [ -d "$dir" ]; then
                basename "$dir"
            fi
        done
        ;;

    "list-proposals")
        # List proposals in a specific subdirectory
        subdir="$2"
        if [ -z "$subdir" ]; then
            echo "Usage: $0 list-proposals SUBDIRECTORY"
            exit 1
        fi
        ls -1 "$ACTIVE_DIR/$subdir/"*.md 2>/dev/null || true
        ;;

    "extract-rule-id")
        # Extract rule ID from proposal filename
        # e.g., P2-MEM05-C-implementation.md -> MEM05-C
        proposal="$2"
        if [ -z "$proposal" ]; then
            echo "Usage: $0 extract-rule-id PROPOSAL_FILENAME"
            exit 1
        fi
        basename "$proposal" | sed 's/^P[0-9]-//' | sed 's/-implementation\.md$//'
        ;;

    "verify-precommit")
        # Verify pre-commit is installed
        if ! command -v pre-commit &> /dev/null; then
            echo "ERROR: pre-commit not installed"
            echo "Install with: pip install pre-commit"
            exit 1
        fi

        # Check if hooks are installed
        if [ ! -f ".git/hooks/pre-commit" ]; then
            echo "WARNING: pre-commit hooks not installed"
            echo "Installing hooks..."
            pre-commit install
        fi

        echo "OK: pre-commit hooks installed"
        ;;

    "create-branch")
        # Create work session branch: claude-work-active-{SUBDIR}-{DATE}
        subdir="$2"
        if [ -z "$subdir" ]; then
            echo "Usage: $0 create-branch SUBDIRECTORY"
            exit 1
        fi

        branch_name="claude-work-active-${subdir}-$(date +%Y%m%d)"

        # Check if already on this branch
        current=$(git branch --show-current)
        if [ "$current" = "$branch_name" ]; then
            echo "Already on branch: $branch_name"
            exit 0
        fi

        # Check if branch exists
        if git show-ref --verify --quiet "refs/heads/$branch_name"; then
            echo "Checking out existing branch: $branch_name"
            git checkout "$branch_name"
        else
            echo "Creating new branch: $branch_name"
            git checkout -b "$branch_name"
        fi
        echo "Now on branch: $branch_name"
        ;;

    "lock-rule")
        # Lock all files except specified rule (from claude_mode_impl_rule.sh)
        rule_id="$2"
        if [ -z "$rule_id" ]; then
            echo "Usage: $0 lock-rule RULE_ID"
            exit 1
        fi

        # Find the rule directory
        rule_dir=$(find src/rules/cert_c -type d -name "$rule_id" | head -1)
        if [ -z "$rule_dir" ]; then
            echo "Error: Rule $rule_id not found in src/rules/cert_c/"
            exit 1
        fi

        echo "Locking files for rule-scoped implementation of $rule_id..."

        # Lock ALL implementations
        find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 444 {} \;

        # Lock ALL test files
        find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 444 {} \;

        # Lock utilities
        find src/utility/cert_c -type f -name "*.rs" -exec chmod 444 {} \; 2>/dev/null

        # Lock mod files
        chmod 444 src/rules/cert_c/mod.rs 2>/dev/null
        chmod 444 src/rules/cert_c/integration.rs 2>/dev/null
        chmod 444 src/utility/cert_c/mod.rs 2>/dev/null
        chmod 444 src/utility/mod.rs 2>/dev/null

        # Unlock ONLY the specified rule's implementation
        find "$rule_dir" -type f -name "*_c.rs" -exec chmod 644 {} \;

        # Unlock the rule's TOML
        find "$rule_dir" -type f -name "*.toml" -exec chmod 644 {} \; 2>/dev/null

        echo "✅ Rule $rule_id implementation is UNLOCKED"
        echo "   All other files are LOCKED (read-only)"
        ;;

    "lock-rule-utils")
        # Lock all except rule AND unlock utilities
        rule_id="$2"
        if [ -z "$rule_id" ]; then
            echo "Usage: $0 lock-rule-utils RULE_ID"
            exit 1
        fi

        # First do the standard rule lock
        "$0" lock-rule "$rule_id"

        # Then unlock utilities
        echo "Unlocking utility files..."
        find src/utility/cert_c -type f -name "*.rs" -exec chmod 644 {} \;
        chmod 644 src/utility/cert_c/mod.rs 2>/dev/null
        chmod 644 src/utility/mod.rs 2>/dev/null

        echo "✅ Utility files are now UNLOCKED for editing"
        ;;

    "unlock-all")
        # Reset all file permissions (from claude_mode_reset.sh)
        echo "Resetting file permissions..."

        # Unlock all Rust implementation files
        find src/rules/cert_c -type f -name "*_c.rs" -exec chmod 644 {} \;

        # Unlock utility files
        find src/utility/cert_c -type f -name "*.rs" -exec chmod 644 {} \; 2>/dev/null

        # Unlock mod.rs and integration.rs files
        chmod 644 src/rules/cert_c/mod.rs 2>/dev/null
        chmod 644 src/rules/cert_c/integration.rs 2>/dev/null
        chmod 644 src/utility/cert_c/mod.rs 2>/dev/null
        chmod 644 src/utility/mod.rs 2>/dev/null

        # Unlock all C test files
        find src/rules/cert_c -type f -path "*/tests/*" -name "*.c" -exec chmod 644 {} \;

        echo "✅ All files unlocked"
        ;;

    *)
        echo "Usage: $0 COMMAND [ARGS]"
        echo ""
        echo "Proposal Discovery:"
        echo "  count-subdirs              Count proposals in each ACTIVE subdirectory"
        echo "  list-subdirs               List subdirectory names"
        echo "  list-proposals SUBDIR      List proposals in a subdirectory"
        echo "  extract-rule-id FILE       Extract rule ID from proposal filename"
        echo ""
        echo "Branch & Safety:"
        echo "  verify-precommit           Verify pre-commit hooks are installed"
        echo "  create-branch SUBDIR       Create work session branch (claude-work-active-SUBDIR-DATE)"
        echo ""
        echo "File Locking (Claude Modes):"
        echo "  lock-rule RULE_ID          Lock all files except the specified rule"
        echo "  lock-rule-utils RULE_ID    Lock to rule + unlock utilities"
        echo "  unlock-all                 Restore write permissions to all files"
        exit 1
        ;;
esac
