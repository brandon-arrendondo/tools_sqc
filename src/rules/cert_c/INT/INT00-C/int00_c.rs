use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils::get_node_text;
use tree_sitter::Node;

pub struct Int00C;

impl CertRule for Int00C {
    fn rule_id(&self) -> &'static str {
        "INT00-C"
    }

    fn description(&self) -> &'static str {
        "Understand the data model used by your implementation(s)"
    }

    fn severity(&self) -> Severity {
        Severity::Medium
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Rule
    }

    fn cert_id(&self) -> &'static str {
        "INT00-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();
        self.check_node(node, source, &mut violations);
        violations
    }
}

impl Int00C {
    fn check_node(&self, node: &Node, source: &str, violations: &mut Vec<RuleViolation>) {
        // Check for format specifier mismatches in scanf/printf family
        if node.kind() == "call_expression" {
            if let Some(func) = node.child_by_field_name("function") {
                let func_name = get_node_text(&func, source);

                if self.is_format_function(&func_name) {
                    self.check_format_specifiers(node, source, violations);
                }
            }
        }

        // Check for unsafe type size assumptions (cast + multiplication)
        if node.kind() == "assignment_expression" {
            self.check_unsafe_cast_multiplication(node, source, violations);
        }

        // Recursively check children
        let mut cursor = node.walk();
        for child in node.children(&mut cursor) {
            self.check_node(&child, source, violations);
        }
    }

    /// Check if function is a format function (printf/scanf family)
    fn is_format_function(&self, func_name: &str) -> bool {
        matches!(
            func_name.trim(),
            "printf"
                | "fprintf"
                | "sprintf"
                | "snprintf"
                | "vprintf"
                | "vfprintf"
                | "vsprintf"
                | "vsnprintf"
                | "scanf"
                | "fscanf"
                | "sscanf"
                | "vscanf"
                | "vfscanf"
                | "vsscanf"
        )
    }

    /// Check format specifiers match variable types
    fn check_format_specifiers(
        &self,
        call_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        // Get function name to determine argument positions
        let func_name = if let Some(func) = call_node.child_by_field_name("function") {
            get_node_text(&func, source).trim().to_string()
        } else {
            return;
        };

        // Determine which argument is the format string
        // fprintf, fscanf, etc. have file pointer first, then format string
        // printf, scanf have format string first
        let format_arg_index = if matches!(
            func_name.as_str(),
            "fprintf"
                | "fscanf"
                | "vfprintf"
                | "vfscanf"
                | "snprintf"
                | "vsnprintf"
                | "sprintf"
                | "vsprintf"
                | "sscanf"
                | "vsscanf"
        ) {
            1 // Format string is second argument (index 1)
        } else {
            0 // Format string is first argument (index 0)
        };

        if let Some(args) = call_node.child_by_field_name("arguments") {
            let mut format_string_node: Option<Node> = None;
            let mut var_args: Vec<Node> = Vec::new();
            let mut arg_index = 0;

            // Parse arguments to find format string and variable arguments
            for i in 0..args.child_count() {
                if let Some(child) = args.child(i) {
                    if child.kind() != "," && child.kind() != "(" && child.kind() != ")" {
                        if arg_index == format_arg_index {
                            format_string_node = Some(child);
                        } else if arg_index > format_arg_index {
                            var_args.push(child);
                        }
                        arg_index += 1;
                    }
                }
            }

            if let Some(fmt_node) = format_string_node {
                let fmt_str = get_node_text(&fmt_node, source);
                if fmt_str.starts_with('"') {
                    let format_specs = self.extract_format_specifiers(&fmt_str);
                    self.validate_format_specifiers(
                        &format_specs,
                        &var_args,
                        source,
                        call_node,
                        violations,
                    );
                }
            }
        }
    }

    /// Extract format specifiers from format string
    fn extract_format_specifiers(&self, format_str: &str) -> Vec<String> {
        let mut specs = Vec::new();
        let chars: Vec<char> = format_str.chars().collect();
        let mut i = 0;

        while i < chars.len() {
            if chars[i] == '%' && i + 1 < chars.len() {
                if chars[i + 1] == '%' {
                    i += 2; // Skip %%
                    continue;
                }

                let mut spec = String::from("%");
                i += 1;

                // Skip flags, width, precision
                while i < chars.len()
                    && (chars[i].is_ascii_digit()
                        || chars[i] == '.'
                        || chars[i] == '-'
                        || chars[i] == '+'
                        || chars[i] == ' '
                        || chars[i] == '#'
                        || chars[i] == '0')
                {
                    spec.push(chars[i]);
                    i += 1;
                }

                // Length modifier
                if i < chars.len() {
                    if chars[i] == 'h' {
                        spec.push('h');
                        i += 1;
                        if i < chars.len() && chars[i] == 'h' {
                            spec.push('h');
                            i += 1;
                        }
                    } else if chars[i] == 'l' {
                        spec.push('l');
                        i += 1;
                        if i < chars.len() && chars[i] == 'l' {
                            spec.push('l');
                            i += 1;
                        }
                    } else if matches!(chars[i], 'L' | 'j' | 'z' | 't') {
                        spec.push(chars[i]);
                        i += 1;
                    }
                }

                // Conversion specifier
                if i < chars.len()
                    && matches!(
                        chars[i],
                        'd' | 'i'
                            | 'o'
                            | 'u'
                            | 'x'
                            | 'X'
                            | 'f'
                            | 'F'
                            | 'e'
                            | 'E'
                            | 'g'
                            | 'G'
                            | 'a'
                            | 'A'
                            | 'c'
                            | 's'
                            | 'p'
                            | 'n'
                    )
                {
                    spec.push(chars[i]);
                    specs.push(spec);
                    i += 1;
                } else {
                    i += 1;
                }
            } else {
                i += 1;
            }
        }

        specs
    }

    /// Validate format specifiers match variable types
    fn validate_format_specifiers(
        &self,
        format_specs: &[String],
        var_args: &[Node],
        source: &str,
        call_node: &Node,
        violations: &mut Vec<RuleViolation>,
    ) {
        for (idx, spec) in format_specs.iter().enumerate() {
            if idx >= var_args.len() {
                continue; // Not enough arguments
            }

            let var_node = &var_args[idx];
            let var_type = self.infer_variable_type(var_node, source);

            // Check for type mismatches
            let has_mismatch = if var_type != "unknown" {
                !self.format_matches_type(spec, &var_type)
            } else {
                // If we can't determine the type, check for known dangerous patterns
                // %ld, %lld, %lu, %llu make assumptions about type sizes
                matches!(spec.as_str(), "%ld" | "%lld" | "%lu" | "%llu")
            };

            if has_mismatch {
                let message = if var_type != "unknown" {
                    format!(
                        "Format specifier '{}' does not match variable type '{}' - violates data model assumptions",
                        spec, var_type
                    )
                } else {
                    format!(
                        "Format specifier '{}' makes assumptions about type sizes - violates data model assumptions",
                        spec
                    )
                };

                violations.push(RuleViolation {
                    rule_id: self.rule_id().to_string(),
                    message,
                    severity: self.severity(),
                    line: call_node.start_position().row + 1,
                    column: call_node.start_position().column + 1,
                    file_path: String::new(),
                    suggestion: Some(format!(
                        "Use correct format specifier or verify data model assumptions with static assertions"
                    )),
                    requires_manual_review: None,
                });
            }
        }
    }

    /// Infer variable type from AST node
    fn infer_variable_type(&self, node: &Node, source: &str) -> String {
        // Extract variable name
        let var_name = if node.kind() == "unary_expression" {
            // For address-of expressions (&x), get the operand
            if let Some(op) = node.child_by_field_name("operator") {
                if get_node_text(&op, source).trim() == "&" {
                    if let Some(operand) = node.child_by_field_name("argument") {
                        get_node_text(&operand, source).trim().to_string()
                    } else {
                        return "unknown".to_string();
                    }
                } else {
                    return "unknown".to_string();
                }
            } else {
                return "unknown".to_string();
            }
        } else {
            get_node_text(node, source).trim().to_string()
        };

        // Simple text-based search for declaration patterns
        // Look for patterns like "int x", "unsigned long y", etc.
        self.find_type_in_source(&var_name, source)
    }

    /// Find variable type by searching source text for declarations
    fn find_type_in_source(&self, var_name: &str, source: &str) -> String {
        // Normalize whitespace in source for easier matching
        let normalized_source = source.split_whitespace().collect::<Vec<_>>().join(" ");

        // Look for common declaration patterns in order of specificity
        let type_patterns = [
            (
                "unsigned long long",
                vec![format!("unsigned long long {}", var_name)],
            ),
            (
                "signed long long",
                vec![format!("signed long long {}", var_name)],
            ),
            ("long long", vec![format!("long long {}", var_name)]),
            ("unsigned long", vec![format!("unsigned long {}", var_name)]),
            ("signed long", vec![format!("signed long {}", var_name)]),
            ("unsigned int", vec![format!("unsigned int {}", var_name)]),
            ("signed int", vec![format!("signed int {}", var_name)]),
            (
                "unsigned short",
                vec![format!("unsigned short {}", var_name)],
            ),
            ("signed short", vec![format!("signed short {}", var_name)]),
            ("unsigned char", vec![format!("unsigned char {}", var_name)]),
            ("signed char", vec![format!("signed char {}", var_name)]),
            ("long double", vec![format!("long double {}", var_name)]),
            ("long", vec![format!("long {}", var_name)]),
            ("short", vec![format!("short {}", var_name)]),
            ("int", vec![format!("int {}", var_name)]),
            ("char", vec![format!("char {}", var_name)]),
            ("float", vec![format!("float {}", var_name)]),
            ("double", vec![format!("double {}", var_name)]),
        ];

        for (type_name, patterns) in &type_patterns {
            for pattern in patterns {
                // Check if the pattern appears followed by ; or ,
                let pattern_with_semi = format!("{};", pattern);
                let pattern_with_comma = format!("{},", pattern);
                if normalized_source.contains(&pattern_with_semi)
                    || normalized_source.contains(&pattern_with_comma)
                {
                    return type_name.to_string();
                }
            }
        }

        "unknown".to_string()
    }

    /// Find declaration in a specific scope
    fn find_declaration_in_scope(
        &self,
        scope: &Node,
        var_name: &str,
        source: &str,
    ) -> Option<String> {
        let mut cursor = scope.walk();
        for child in scope.children(&mut cursor) {
            if child.kind() == "declaration" {
                let decl_text = get_node_text(&child, source);
                // Simple check: does this declaration contain the variable name?
                if decl_text.contains(var_name) {
                    // Extract type from declaration
                    return Some(self.extract_type_from_declaration(&child, source));
                }
            }
            // Recursively search in child scopes
            if let Some(found_type) = self.find_declaration_in_scope(&child, var_name, source) {
                return Some(found_type);
            }
        }
        None
    }

    /// Find declarator matching variable name
    fn find_declarator<'a>(
        &self,
        decl_node: &Node<'a>,
        var_name: &str,
        source: &str,
    ) -> Option<Node<'a>> {
        let mut cursor = decl_node.walk();
        for child in decl_node.children(&mut cursor) {
            if child.kind() == "init_declarator" || child.kind() == "identifier" {
                let text = get_node_text(&child, source);
                if text.contains(var_name) {
                    return Some(child);
                }
            }
        }
        None
    }

    /// Extract type from declaration node
    fn extract_type_from_declaration(&self, decl_node: &Node, source: &str) -> String {
        if let Some(type_node) = decl_node.child_by_field_name("type") {
            return get_node_text(&type_node, source).trim().to_string();
        }

        // Fallback: get first child that looks like a type
        let mut cursor = decl_node.walk();
        for child in decl_node.children(&mut cursor) {
            if matches!(
                child.kind(),
                "primitive_type" | "type_identifier" | "sized_type_specifier"
            ) {
                return get_node_text(&child, source).trim().to_string();
            }
        }

        // Last resort: parse the declaration text directly
        let decl_text = get_node_text(decl_node, source);
        // Match common type patterns: "int x", "unsigned int x", "long x", etc.
        if let Some(type_part) = decl_text.split_whitespace().next() {
            // Simple type extraction
            if matches!(
                type_part,
                "int" | "char" | "short" | "long" | "float" | "double" | "unsigned" | "signed"
            ) {
                // For types like "unsigned int", get both words
                let words: Vec<&str> = decl_text.split_whitespace().collect();
                if words.len() >= 2 {
                    if words[0] == "unsigned" || words[0] == "signed" {
                        return format!("{} {}", words[0], words[1]);
                    }
                }
                return type_part.to_string();
            }
        }

        "unknown".to_string()
    }

    /// Check if format specifier matches type
    fn format_matches_type(&self, spec: &str, var_type: &str) -> bool {
        match spec {
            // %d, %i - signed int
            "%d" | "%i" => var_type == "int" || var_type == "signed int",
            // %ld, %li - signed long
            "%ld" | "%li" => var_type.contains("long") && !var_type.contains("long long"),
            // %lld, %lli - signed long long
            "%lld" | "%lli" => var_type.contains("long long"),
            // %u - unsigned int
            "%u" => var_type == "unsigned int" || var_type == "unsigned",
            // %lu - unsigned long
            "%lu" => var_type == "unsigned long" && !var_type.contains("long long"),
            // %llu - unsigned long long
            "%llu" => var_type == "unsigned long long",
            // %f, %e, %g - float/double
            "%f" | "%e" | "%g" | "%F" | "%E" | "%G" => {
                var_type.contains("float") || var_type.contains("double")
            }
            // %lf - double (in scanf)
            "%lf" => var_type.contains("double"),
            // %c - char
            "%c" => var_type == "char" || var_type == "signed char" || var_type == "unsigned char",
            // %s - char*
            "%s" => var_type.contains("char") && var_type.contains("*"),
            // %p - pointer
            "%p" => var_type.contains("*") || var_type == "void *",
            _ => true, // Unknown specifier, don't flag
        }
    }

    /// Check for unsafe cast + multiplication patterns
    fn check_unsafe_cast_multiplication(
        &self,
        assign_node: &Node,
        source: &str,
        violations: &mut Vec<RuleViolation>,
    ) {
        if let Some(right) = assign_node.child_by_field_name("right") {
            // Pattern 1: (unsigned long)(a * b) - cast wrapping multiplication
            if right.kind() == "cast_expression" {
                if let Some(cast_type) = right.child_by_field_name("type") {
                    let cast_type_name = get_node_text(&cast_type, source);

                    // Check if casting to a larger type like unsigned long
                    if cast_type_name.contains("unsigned long")
                        && !cast_type_name.contains("long long")
                    {
                        // Check if value being cast is a multiplication
                        if let Some(value) = right.child_by_field_name("value") {
                            if value.kind() == "binary_expression" {
                                if let Some(op) = value.child_by_field_name("operator") {
                                    if get_node_text(&op, source).trim() == "*" {
                                        violations.push(RuleViolation {
                                            rule_id: self.rule_id().to_string(),
                                            message: "Casting multiplication result to larger type without verifying result fits - violates data model assumptions".to_string(),
                                            severity: self.severity(),
                                            line: assign_node.start_position().row + 1,
                                            column: assign_node.start_position().column + 1,
                                            file_path: String::new(),
                                            suggestion: Some("Use largest available type (e.g., uintmax_t) with preprocessor guards to ensure result fits".to_string()),
                                            requires_manual_review: None,
                                        });
                                    }
                                }
                            }
                        }
                    }
                }
            }
            // Pattern 2: (unsigned long)a * b - cast one operand, then multiply
            else if right.kind() == "binary_expression" {
                if let Some(op) = right.child_by_field_name("operator") {
                    if get_node_text(&op, source).trim() == "*" {
                        // Check if either operand is a cast to unsigned long
                        let left = right.child_by_field_name("left");
                        let right_op = right.child_by_field_name("right");

                        let has_unsigned_long_cast = [left, right_op].iter().any(|operand| {
                            if let Some(op_node) = operand {
                                if op_node.kind() == "cast_expression" {
                                    if let Some(cast_type) = op_node.child_by_field_name("type") {
                                        let type_text = get_node_text(&cast_type, source);
                                        return type_text.contains("unsigned long")
                                            && !type_text.contains("long long");
                                    }
                                }
                            }
                            false
                        });

                        if has_unsigned_long_cast {
                            violations.push(RuleViolation {
                                rule_id: self.rule_id().to_string(),
                                message: "Casting operand to larger type in multiplication without verifying result fits - violates data model assumptions".to_string(),
                                severity: self.severity(),
                                line: assign_node.start_position().row + 1,
                                column: assign_node.start_position().column + 1,
                                file_path: String::new(),
                                suggestion: Some("Use largest available type (e.g., uintmax_t) with preprocessor guards to ensure result fits".to_string()),
                                requires_manual_review: None,
                            });
                        }
                    }
                }
            }
        }
    }
}
