#!/bin/bash
# Unified helper scripts for work-active workflow and Claude modes
# Generalized file locking system - lock everything except specified files

set -e

ACTIVE_DIR="AGENTS/PROPOSALS/ACTIVE"
LOCK_CONFIG=".claude/lock-list.yaml"

# Read lock directories from config file
get_lock_directories() {
    if [ ! -f "$LOCK_CONFIG" ]; then
        echo "Error: Lock config file not found: $LOCK_CONFIG" >&2
        exit 1
    fi

    # Extract lock_directories from YAML
    awk '
    /^lock_directories:/ { in_section=1; next; }
    in_section && /^  - / {
        gsub(/^  - /, "");
        gsub(/"/, "");
        print;
        next;
    }
    in_section && /^[^ ]/ { in_section=0; }
    ' "$LOCK_CONFIG"
}

# Build find exclusion arguments from config
get_exclusions() {
    if [ ! -f "$LOCK_CONFIG" ]; then
        return
    fi

    # Extract exclude_always patterns from YAML
    awk '
    /^exclude_always:/ { in_section=1; next; }
    in_section && /^  - / {
        gsub(/^  - /, "");
        gsub(/"/, "");
        # Convert to find -not -path format
        if ($0 !~ /\*/) {
            # Directory or specific file
            printf " -not -path \"./%s\" -not -path \"./%s*\"", $0, $0;
        }
        next;
    }
    in_section && /^[^ ]/ { in_section=0; }
    ' "$LOCK_CONFIG"
}

# Lock all files in configured scope with chmod 000
lock_all_in_scope() {
    echo "Reading lock configuration from $LOCK_CONFIG..."

    # Get directories to lock
    lock_dirs=$(get_lock_directories)

    if [ -z "$lock_dirs" ]; then
        echo "Error: No lock_directories specified in $LOCK_CONFIG"
        exit 1
    fi

    echo "Locking files in configured directories:"
    echo "$lock_dirs" | while read -r dir; do
        echo "  - $dir"
    done

    # Get exclusion patterns
    exclusions=$(get_exclusions)

    # Lock files in each configured directory
    echo "$lock_dirs" | while read -r dir; do
        if [ -d "$dir" ]; then
            eval "find \"$dir\" -type f $exclusions -exec chmod 000 {} \\; 2>/dev/null || true"
        fi
    done

    echo "✅ Files locked in configured scope"
}

# Unlock specific files (restore write permissions)
unlock_files() {
    for file in "$@"; do
        if [ -f "$file" ]; then
            chmod 644 "$file"
        else
            # Try glob expansion
            for expanded in $file; do
                if [ -f "$expanded" ]; then
                    chmod 644 "$expanded"
                fi
            done
        fi
    done
}

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

    "lock-from-proposal")
        # Lock all files except those specified in proposal YAML front matter
        # NOTE: This feature is NOT YET IMPLEMENTED in existing proposals
        #       Current proposals do not have unlock_files in YAML front matter
        proposal_file="$2"
        if [ -z "$proposal_file" ] || [ ! -f "$proposal_file" ]; then
            echo "Usage: $0 lock-from-proposal PROPOSAL_FILE"
            echo "Error: Proposal file not found or not specified"
            exit 1
        fi

        echo "⚠️  WARNING: lock-from-proposal requires unlock_files in YAML front matter"
        echo "⚠️  This is NOT YET IMPLEMENTED in existing proposals"
        echo "⚠️  Use 'lock-except FILE1 FILE2...' for manual file specification"
        echo ""
        # echo "Reading unlock_files from $proposal_file..."

        # # Extract unlock_files from YAML front matter
        # # Look for lines between --- markers, find unlock_files section
        # unlock_files_list=$(awk '
        #     BEGIN { in_yaml=0; in_unlock=0; }
        #     /^---$/ {
        #         if (in_yaml == 0) { in_yaml=1; next; }
        #         else { exit; }
        #     }
        #     in_yaml && /^unlock_files:/ { in_unlock=1; next; }
        #     in_yaml && in_unlock && /^  - / {
        #         gsub(/^  - /, "");
        #         print;
        #     }
        #     in_yaml && in_unlock && /^[^ ]/ { in_unlock=0; }
        # ' "$proposal_file")

        # if [ -z "$unlock_files_list" ]; then
        #     echo "❌ ERROR: No unlock_files found in proposal YAML front matter"
        #     echo "   This feature requires proposals to be updated with unlock_files section"
        #     echo "   Use 'lock-except FILE1 FILE2...' instead for now"
        #     exit 1
        # fi

        # # Lock all files
        # lock_all_in_scope

        # # Unlock specified files
        # echo "Unlocking specified files:"
        # while IFS= read -r file; do
        #     echo "  - $file"
        #     unlock_files "$file"
        # done <<< "$unlock_files_list"

        # echo "✅ File locking complete"
        # echo "   All files locked with chmod 000 except specified unlock_files"

        echo "❌ ERROR: lock-from-proposal is not yet implemented"
        echo "   Use 'lock-except FILE1 FILE2...' instead"
        exit 1
        ;;

    "lock-for-impl")
        # Lock all files except rule implementation (tests remain locked)
        # Usage: lock-for-impl RULE_ID
        rule_id="$2"
        if [ -z "$rule_id" ]; then
            echo "Usage: $0 lock-for-impl RULE_ID"
            echo "Locks all files except rule implementation (tests LOCKED)"
            exit 1
        fi

        # Find the rule directory
        rule_dir=$(find src/rules/cert_c -type d -name "$rule_id" | head -1)
        if [ -z "$rule_dir" ]; then
            echo "Error: Rule $rule_id not found in src/rules/cert_c/"
            exit 1
        fi

        echo "Locking for implementation mode: $rule_id"

        # Lock all files
        lock_all_in_scope

        # Unlock rule implementation files
        echo "Unlocking implementation files:"
        find "$rule_dir" -type f -name "*_c.rs" -exec echo "  - {}" \; -exec chmod 644 {} \;
        find "$rule_dir" -type f -name "*.toml" -exec echo "  - {}" \; -exec chmod 644 {} \;

        echo "✅ Implementation mode active"
        echo "   Rule $rule_id implementation files unlocked"
        echo "   Test files remain LOCKED (chmod 000)"
        ;;

    "lock-for-test")
        # Lock all files except rule test files (implementation locked)
        # Usage: lock-for-test RULE_ID
        rule_id="$2"
        if [ -z "$rule_id" ]; then
            echo "Usage: $0 lock-for-test RULE_ID"
            echo "Locks all files except rule test files (implementation LOCKED)"
            exit 1
        fi

        # Find the rule directory
        rule_dir=$(find src/rules/cert_c -type d -name "$rule_id" | head -1)
        if [ -z "$rule_dir" ]; then
            echo "Error: Rule $rule_id not found in src/rules/cert_c/"
            exit 1
        fi

        echo "Locking for test editing mode: $rule_id"

        # Lock all files
        lock_all_in_scope

        # Unlock test files only
        echo "Unlocking test files:"
        find "$rule_dir" -type f -path "*/tests/*" -name "*.c" -exec echo "  - {}" \; -exec chmod 644 {} \;

        echo "✅ Test editing mode active"
        echo "   Rule $rule_id test files unlocked"
        echo "   Implementation files remain LOCKED (chmod 000)"
        ;;

    "lock-except")
        # Lock all files in scope except specified files
        # Usage: lock-except FILE1 FILE2 FILE3 ...
        shift  # Remove command name

        if [ $# -eq 0 ]; then
            echo "Usage: $0 lock-except FILE1 [FILE2 FILE3 ...]"
            echo "Locks all files in scope (chmod 000) except specified files"
            exit 1
        fi

        # Lock all files
        lock_all_in_scope

        # Unlock specified files
        echo "Unlocking specified files:"
        for file in "$@"; do
            echo "  - $file"
        done
        unlock_files "$@"

        echo "✅ File locking complete"
        echo "   All files locked with chmod 000 except specified files"
        ;;

    "unlock-all")
        # Reset file permissions in configured directories
        echo "Reading lock configuration from $LOCK_CONFIG..."

        # Get directories to unlock
        lock_dirs=$(get_lock_directories)

        if [ -z "$lock_dirs" ]; then
            echo "Error: No lock_directories specified in $LOCK_CONFIG"
            exit 1
        fi

        echo "Restoring permissions in configured directories:"
        echo "$lock_dirs" | while read -r dir; do
            echo "  - $dir"
        done

        # Unlock files in each configured directory
        echo "$lock_dirs" | while read -r dir; do
            if [ -d "$dir" ]; then
                find "$dir" -type f -exec chmod 644 {} \; 2>/dev/null || true
            fi
        done

        # Restore execute permissions on scripts
        find scripts -type f -name "*.sh" -exec chmod 755 {} \; 2>/dev/null || true

        echo "✅ Files unlocked in configured scope (644), scripts restored to 755"
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
        echo "File Locking:"
        echo "  lock-for-impl RULE_ID      Lock all except rule implementation (tests LOCKED)"
        echo "  lock-for-test RULE_ID      Lock all except rule test files (impl LOCKED)"
        echo "  lock-except FILE1 ...      Lock configured dirs except specified files"
        echo "  unlock-all                 Restore write permissions in configured dirs"
        echo "  lock-from-proposal FILE    [NOT YET IMPLEMENTED] Read YAML unlock_files"
        echo ""
        echo "Notes:"
        echo "  - Lock scope configured in .claude/lock-list.yaml (default: src/)"
        echo "  - Exclusions: .git/, target/, tmp/, scripts/ (see config)"
        echo "  - File locking uses chmod 000 (complete lockout - no read or write)"
        echo "  - lock-for-impl: Implementation mode (tests locked, get context from proposal)"
        echo "  - lock-for-test: Test editing mode (implementation locked)"
        echo "  - lock-except: Manual mode (specify exact files to unlock)"
        exit 1
        ;;
esac
