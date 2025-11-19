#!/bin/bash
# Helper scripts for distributed review workflow (/gather-opinions and /review-staged)

set -e

STAGED_DIR="AGENTS/PROPOSALS/STAGED"
COMPLETE_DIR="AGENTS/PROPOSALS/COMPLETE"
ACTIVE_DIR="AGENTS/PROPOSALS/ACTIVE"
STALLED_DIR="AGENTS/PROPOSALS/STALLED"

# Get reviewer name from git config
get_reviewer_name() {
    git config user.name | tr ' ' '-' | tr '[:upper:]' '[:lower:]'
}

# Add opinion to proposal frontmatter
# Usage: add-opinion PROPOSAL_FILE PERSONA OPINION COMMENT
add_opinion() {
    local proposal_file="$1"
    local persona="$2"
    local opinion="$3"
    local comment="$4"

    if [ ! -f "$proposal_file" ]; then
        echo "ERROR: Proposal file not found: $proposal_file"
        exit 1
    fi

    local reviewer=$(get_reviewer_name)
    local date=$(date +%Y-%m-%d)
    local timestamp=$(date -Iseconds)

    # Check if reviews section exists
    if ! grep -q "^reviews:" "$proposal_file"; then
        # Find where to insert reviews section (after YAML front matter or before first ##)
        # Insert after the status/priority block, before first ## section

        # Create reviews section
        local reviews_section="
reviews:
  - reviewer: $reviewer
    persona: $persona
    date: $date
    timestamp: $timestamp
    opinion: $opinion
    comment: \"$comment\"
"

        # Find insertion point (after front matter, before first ##)
        local insert_line=$(grep -n "^## " "$proposal_file" | head -1 | cut -d: -f1)

        if [ -z "$insert_line" ]; then
            # No ## found, append to end
            echo "$reviews_section" >> "$proposal_file"
        else
            # Insert before first ##
            {
                head -n $((insert_line - 1)) "$proposal_file"
                echo "$reviews_section"
                tail -n +$insert_line "$proposal_file"
            } > "${proposal_file}.tmp"
            mv "${proposal_file}.tmp" "$proposal_file"
        fi
    else
        # Append to existing reviews section
        local review_entry="  - reviewer: $reviewer
    persona: $persona
    date: $date
    timestamp: $timestamp
    opinion: $opinion
    comment: \"$comment\""

        # Find last line of reviews section
        local reviews_start=$(grep -n "^reviews:" "$proposal_file" | cut -d: -f1)
        local next_section=$(tail -n +$((reviews_start + 1)) "$proposal_file" | grep -n "^[a-z_]*:" | head -1 | cut -d: -f1)

        if [ -n "$next_section" ]; then
            # Insert before next section
            local insert_at=$((reviews_start + next_section))
            {
                head -n $((insert_at - 1)) "$proposal_file"
                echo "$review_entry"
                tail -n +$insert_at "$proposal_file"
            } > "${proposal_file}.tmp"
            mv "${proposal_file}.tmp" "$proposal_file"
        else
            # Append to end of file
            echo "$review_entry" >> "$proposal_file"
        fi
    fi

    echo "✓ Opinion added to $proposal_file"
    echo "  Reviewer: $reviewer ($persona)"
    echo "  Opinion: $opinion"
}

# Analyze opinion coverage across all STAGED proposals
analyze_coverage() {
    echo "=== Review Coverage Analysis ==="
    echo ""

    local total=$(ls -1 "$STAGED_DIR"/*.md 2>/dev/null | wc -l)
    echo "Total proposals in STAGED: $total"
    echo ""

    local coverage_0=0
    local coverage_1=0
    local coverage_2=0
    local coverage_3plus=0

    for proposal in "$STAGED_DIR"/*.md; do
        if [ ! -f "$proposal" ]; then
            continue
        fi

        local review_count=$(grep -c "^  - reviewer:" "$proposal" 2>/dev/null | tr -d '\n')
        review_count=${review_count:-0}

        if [ "$review_count" -eq 0 ]; then
            coverage_0=$((coverage_0 + 1))
        elif [ "$review_count" -eq 1 ]; then
            coverage_1=$((coverage_1 + 1))
        elif [ "$review_count" -eq 2 ]; then
            coverage_2=$((coverage_2 + 1))
        else
            coverage_3plus=$((coverage_3plus + 1))
        fi
    done

    echo "Coverage:"
    echo "  3+ reviewers: $coverage_3plus proposals (good coverage)"
    echo "  2 reviewers: $coverage_2 proposals (minimal coverage)"
    echo "  1 reviewer: $coverage_1 proposals (needs more opinions)"
    echo "  0 reviewers: $coverage_0 proposals (NOT REVIEWED)"
    echo ""

    if [ $coverage_0 -gt 0 ]; then
        echo "⚠️  WARNING: $coverage_0 proposals have no reviews yet"
    fi

    if [ $coverage_1 -gt 0 ]; then
        echo "ℹ️  NOTE: $coverage_1 proposals have only 1 review (recommend 2+ for consensus)"
    fi

    echo ""
    echo "Recommendation:"
    if [ $((coverage_2 + coverage_3plus)) -gt 0 ]; then
        echo "  ✅ $((coverage_2 + coverage_3plus)) proposals ready for /review-staged (2+ reviewers)"
    fi
    if [ $((coverage_0 + coverage_1)) -gt 0 ]; then
        echo "  ⚠️  $((coverage_0 + coverage_1)) proposals need more opinions"
    fi
}

# Analyze opinions to find consensus
analyze_opinions() {
    echo "=== Opinion Analysis ==="
    echo ""

    local total=$(ls -1 "$STAGED_DIR"/*.md 2>/dev/null | wc -l)
    local total_opinions=0
    local looks_good=0
    local needs_review=0
    local blocked=0

    for proposal in "$STAGED_DIR"/*.md; do
        if [ ! -f "$proposal" ]; then
            continue
        fi

        # Count opinions by type
        local lg=$(grep -c "opinion: LOOKS_GOOD" "$proposal" 2>/dev/null | tr -d '\n')
        lg=${lg:-0}
        local nr=$(grep -c "opinion: NEEDS_REVIEW" "$proposal" 2>/dev/null | tr -d '\n')
        nr=${nr:-0}
        local bl=$(grep -c "opinion: BLOCKED" "$proposal" 2>/dev/null | tr -d '\n')
        bl=${bl:-0}

        looks_good=$((looks_good + lg))
        needs_review=$((needs_review + nr))
        blocked=$((blocked + bl))
        total_opinions=$((total_opinions + lg + nr + bl))
    done

    echo "Total proposals: $total"
    echo "Total opinions: $total_opinions"
    echo ""
    echo "Opinion Distribution:"
    echo "  ✅ LOOKS_GOOD: $looks_good opinions ($(( looks_good * 100 / (total_opinions > 0 ? total_opinions : 1) ))%)"
    echo "  ⚠️  NEEDS_REVIEW: $needs_review opinions ($(( needs_review * 100 / (total_opinions > 0 ? total_opinions : 1) ))%)"
    echo "  🛑 BLOCKED: $blocked opinions ($(( blocked * 100 / (total_opinions > 0 ? total_opinions : 1) ))%)"
    echo ""

    # Find proposals with consensus
    local strong_consensus=0
    local weak_consensus=0
    local no_consensus=0

    for proposal in "$STAGED_DIR"/*.md; do
        if [ ! -f "$proposal" ]; then
            continue
        fi

        local review_count=$(grep -c "^  - reviewer:" "$proposal" 2>/dev/null | tr -d '\n')
        review_count=${review_count:-0}

        if [ "$review_count" -lt 2 ]; then
            continue  # Skip proposals without enough reviews
        fi

        local lg=$(grep -c "opinion: LOOKS_GOOD" "$proposal" 2>/dev/null | tr -d '\n')
        lg=${lg:-0}
        local nr=$(grep -c "opinion: NEEDS_REVIEW" "$proposal" 2>/dev/null | tr -d '\n')
        nr=${nr:-0}
        local bl=$(grep -c "opinion: BLOCKED" "$proposal" 2>/dev/null | tr -d '\n')
        bl=${bl:-0}

        # Strong consensus: all same opinion
        if [ "$lg" -eq "$review_count" ] || [ "$nr" -eq "$review_count" ] || [ "$bl" -eq "$review_count" ]; then
            strong_consensus=$((strong_consensus + 1))
        # Weak consensus: majority opinion (>50%)
        elif [ "$lg" -gt $((review_count / 2)) ] || [ "$nr" -gt $((review_count / 2)) ] || [ "$bl" -gt $((review_count / 2)) ]; then
            weak_consensus=$((weak_consensus + 1))
        else
            no_consensus=$((no_consensus + 1))
        fi
    done

    echo "Consensus (among proposals with 2+ reviews):"
    echo "  Strong agreement (all same): $strong_consensus proposals"
    echo "  Weak agreement (majority): $weak_consensus proposals"
    echo "  Disagreement: $no_consensus proposals (needs architect decision)"
}

# List proposals by consensus type
list_by_consensus() {
    local filter="$1"  # strong|weak|none|blocked

    if [ -z "$filter" ]; then
        echo "Usage: $0 list-by-consensus [strong|weak|none|blocked]"
        exit 1
    fi

    for proposal in "$STAGED_DIR"/*.md; do
        if [ ! -f "$proposal" ]; then
            continue
        fi

        local basename=$(basename "$proposal")
        local review_count=$(grep -c "^  - reviewer:" "$proposal" 2>/dev/null | tr -d '\n')
        review_count=${review_count:-0}

        if [ "$review_count" -lt 2 ] && [ "$filter" != "blocked" ]; then
            continue  # Skip unless looking for blocked
        fi

        local lg=$(grep -c "opinion: LOOKS_GOOD" "$proposal" 2>/dev/null | tr -d '\n')
        lg=${lg:-0}
        local nr=$(grep -c "opinion: NEEDS_REVIEW" "$proposal" 2>/dev/null | tr -d '\n')
        nr=${nr:-0}
        local bl=$(grep -c "opinion: BLOCKED" "$proposal" 2>/dev/null | tr -d '\n')
        bl=${bl:-0}

        case "$filter" in
            strong)
                # All opinions are the same
                if [ "$lg" -eq "$review_count" ] || [ "$nr" -eq "$review_count" ] || [ "$bl" -eq "$review_count" ]; then
                    if [ "$lg" -eq "$review_count" ]; then
                        echo "✅ $basename (all LOOKS_GOOD, $review_count reviewers)"
                    elif [ "$nr" -eq "$review_count" ]; then
                        echo "⚠️  $basename (all NEEDS_REVIEW, $review_count reviewers)"
                    else
                        echo "🛑 $basename (all BLOCKED, $review_count reviewers)"
                    fi
                fi
                ;;
            weak)
                # Majority opinion
                if [ "$lg" -gt $((review_count / 2)) ]; then
                    echo "✅ $basename (majority LOOKS_GOOD: $lg/$review_count)"
                elif [ "$nr" -gt $((review_count / 2)) ]; then
                    echo "⚠️  $basename (majority NEEDS_REVIEW: $nr/$review_count)"
                elif [ "$bl" -gt $((review_count / 2)) ]; then
                    echo "🛑 $basename (majority BLOCKED: $bl/$review_count)"
                fi
                ;;
            none)
                # No majority (split opinions)
                if [ "$lg" -le $((review_count / 2)) ] && [ "$nr" -le $((review_count / 2)) ] && [ "$bl" -le $((review_count / 2)) ]; then
                    echo "❓ $basename (split: $lg LOOKS_GOOD, $nr NEEDS_REVIEW, $bl BLOCKED)"
                fi
                ;;
            blocked)
                # Any BLOCKED opinions
                if [ "$bl" -gt 0 ]; then
                    echo "🛑 $basename ($bl BLOCKED opinions)"
                fi
                ;;
        esac
    done
}

# Move proposal to COMPLETE
move_to_complete() {
    local proposal_file="$1"

    if [ ! -f "$STAGED_DIR/$proposal_file" ]; then
        echo "ERROR: Proposal not found in STAGED: $proposal_file"
        exit 1
    fi

    git mv "$STAGED_DIR/$proposal_file" "$COMPLETE_DIR/$proposal_file"
    echo "✓ Moved to COMPLETE: $proposal_file"
}

# Move proposal to ACTIVE (with issues)
move_to_active() {
    local proposal_file="$1"

    if [ ! -f "$STAGED_DIR/$proposal_file" ]; then
        echo "ERROR: Proposal not found in STAGED: $proposal_file"
        exit 1
    fi

    git mv "$STAGED_DIR/$proposal_file" "$ACTIVE_DIR/$proposal_file"
    echo "✓ Moved to ACTIVE: $proposal_file"
}

# Move proposal to STALLED
move_to_stalled() {
    local proposal_file="$1"

    if [ ! -f "$STAGED_DIR/$proposal_file" ]; then
        echo "ERROR: Proposal not found in STAGED: $proposal_file"
        exit 1
    fi

    git mv "$STAGED_DIR/$proposal_file" "$STALLED_DIR/$proposal_file"
    echo "✓ Moved to STALLED: $proposal_file"
}

# Show help
show_help() {
    cat << EOF
review_helpers.sh - Distributed review workflow helper

PHASE 1 COMMANDS (gather-opinions):
  get-reviewer-name              Get reviewer name from git config
  add-opinion FILE PERSONA OPINION COMMENT
                                 Add opinion to proposal frontmatter
                                 OPINION: LOOKS_GOOD|NEEDS_REVIEW|BLOCKED
  analyze-coverage               Show review coverage statistics

PHASE 2 COMMANDS (review-staged):
  analyze-opinions               Analyze all opinions and find consensus
  list-by-consensus TYPE         List proposals by consensus type
                                 TYPE: strong|weak|none|blocked
  move-to-complete FILE          Move proposal to COMPLETE/
  move-to-active FILE            Move proposal to ACTIVE/
  move-to-stalled FILE           Move proposal to STALLED/

EXAMPLES:
  # Phase 1: Add your opinion
  $0 add-opinion P1-FIO37-C-implementation.md "Security Auditor" "NEEDS_REVIEW" "Line 142: unwrap() on user input"

  # Phase 2: Analyze and move
  $0 analyze-opinions
  $0 list-by-consensus strong
  $0 move-to-complete P1-FIO37-C-implementation.md
EOF
}

# Main command dispatcher
case "$1" in
    "get-reviewer-name")
        get_reviewer_name
        ;;

    "add-opinion")
        if [ $# -lt 5 ]; then
            echo "Usage: $0 add-opinion PROPOSAL_FILE PERSONA OPINION COMMENT"
            exit 1
        fi
        add_opinion "$2" "$3" "$4" "$5"
        ;;

    "analyze-coverage")
        analyze_coverage
        ;;

    "analyze-opinions")
        analyze_opinions
        ;;

    "list-by-consensus")
        list_by_consensus "$2"
        ;;

    "move-to-complete")
        move_to_complete "$2"
        ;;

    "move-to-active")
        move_to_active "$2"
        ;;

    "move-to-stalled")
        move_to_stalled "$2"
        ;;

    "help"|"--help"|"-h"|"")
        show_help
        ;;

    *)
        echo "ERROR: Unknown command: $1"
        echo ""
        show_help
        exit 1
        ;;
esac
