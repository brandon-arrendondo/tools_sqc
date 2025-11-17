#!/bin/bash
# Generate proposal files for remaining CERT C rules

TEAMS=("ALLY" "BRANDON" "BLAKE" "ERIC" "HUU" "JASON" "TRISTAN")
TOML_FILE="src/rules/cert_c/rules-all.toml"
RULES_FILE="/tmp/shuffled_rules.txt"
BASE_DIR="AGENTS/PROPOSALS/ACTIVE"

# Track assignments for master doc
ASSIGNMENTS_FILE="/tmp/assignments.txt"
> "$ASSIGNMENTS_FILE"

idx=0
total=$(wc -l < "$RULES_FILE")

while IFS= read -r full_rule_id; do
    # Extract rule ID (e.g., cert_c.MEM05-C -> MEM05-C)
    rule_id=$(echo "$full_rule_id" | sed 's/cert_c\.//')

    # Assign to team member (round-robin)
    team_idx=$((idx % 7))
    team="${TEAMS[$team_idx]}"

    # Extract category (e.g., MEM05-C -> MEM)
    category=$(echo "$rule_id" | sed 's/[0-9].*//')

    # Get rule metadata from TOML
    title=$(grep -A 20 "^\[rules\.${full_rule_id}\]" "$TOML_FILE" | grep "^title" | head -1 | sed 's/title = "\(.*\)"/\1/')
    rule_type=$(grep -A 20 "^\[rules\.${full_rule_id}\]" "$TOML_FILE" | grep "^type" | head -1 | sed 's/type = "\(.*\)"/\1/')
    priority=$(grep -A 20 "^\[rules\.${full_rule_id}\]" "$TOML_FILE" | grep "^priority" | head -1 | sed 's/priority = "\(.*\)"/\1/')
    level=$(grep -A 20 "^\[rules\.${full_rule_id}\]" "$TOML_FILE" | grep "^level" | head -1 | sed 's/level = "\(.*\)"/\1/')
    enabled=$(grep -A 20 "^\[rules\.${full_rule_id}\]" "$TOML_FILE" | grep "^enabled" | head -1 | sed 's/enabled = //')

    # Default values if not found
    [ -z "$title" ] && title="$rule_id Implementation"
    [ -z "$rule_type" ] && rule_type="rule"
    [ -z "$priority" ] && priority="L2"
    [ -z "$level" ] && level="L2"
    [ -z "$enabled" ] && enabled="false"

    # Create proposal file
    proposal_file="$BASE_DIR/$team/P2-${rule_id}-implementation.md"

    cat > "$proposal_file" << PROPOSAL
# P2-${rule_id} - ${title}

**Status:** ACTIVE
**Priority:** P2 (Distributed Assignment)
**Created:** 2025-11-17
**Assigned To:** ${team}
**Category:** ${category}
**Estimated Effort:** 10-30 hours

## CERT C Rule Information

**Rule ID:** ${rule_id}
**Type:** ${rule_type}
**CERT Priority:** ${priority}
**Level:** ${level}
**Currently Enabled:** ${enabled}

**Wiki Reference:**
https://wiki.sei.cmu.edu/confluence/display/c/${rule_id}

---

## Task

Implement or verify ${rule_id} with 100% test pass rate and DRY compliance.

### Requirements:
1. Study the CERT C wiki page for ${rule_id}
2. Check if implementation exists in \`src/rules/cert_c/${category}/${rule_id}/\`
3. If exists: verify tests pass, ensure DRY compliance
4. If not exists: implement from scratch following existing patterns
5. Ensure all test cases pass (100% pass rate required)
6. Use shared utilities from \`src/utility/cert_c/\`

---

## Acceptance Criteria

- [ ] Implementation exists and compiles
- [ ] All test cases pass (100% pass rate)
- [ ] Uses get_node_text() and other shared utilities (DRY compliance)
- [ ] Rule enabled in configuration
- [ ] Implementation documented with comments

---

## Implementation Log

(To be filled in during implementation)

---

## Verification

@architect: Pending verification
PROPOSAL

    # Track assignment
    echo "${rule_id}|${team}|${title}" >> "$ASSIGNMENTS_FILE"

    idx=$((idx + 1))

    # Progress indicator
    if [ $((idx % 50)) -eq 0 ]; then
        echo "Processed $idx / $total rules..."
    fi
done < "$RULES_FILE"

echo "Generated $idx proposal files"
echo "Assignments saved to $ASSIGNMENTS_FILE"

# Show distribution
echo ""
echo "Distribution by team:"
for team in "${TEAMS[@]}"; do
    count=$(ls "$BASE_DIR/$team/" 2>/dev/null | wc -l)
    echo "  $team: $count proposals"
done
