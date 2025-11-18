use super::super::{CertRule, RuleViolation};
use crate::manifest::{RuleCategory, Severity};
use crate::utility::cert_c::ast_utils;
use std::collections::HashMap;
use tree_sitter::Node;

pub struct Dcl01C;

impl CertRule for Dcl01C {
    fn rule_id(&self) -> &'static str {
        "DCL01-C"
    }

    fn description(&self) -> &'static str {
        "Do not reuse variable names in subscopes"
    }

    fn severity(&self) -> Severity {
        Severity::Low
    }

    fn category(&self) -> RuleCategory {
        RuleCategory::Recommendation
    }

    fn cert_id(&self) -> &'static str {
        "DCL01-C"
    }

    fn check(&self, node: &Node, source: &str) -> Vec<RuleViolation> {
        let mut violations = Vec::new();

        // Collect all variable declarations at this level and check for shadowing
        check_scope_for_shadowing(
            node,
            source,
            &HashMap::new(),
            &mut violations,
            self.rule_id(),
        );

        violations
    }
}

/// Recursively check scopes for variable name shadowing
///
/// # Arguments
/// * `node` - Current AST node being checked
/// * `source` - Complete source code
/// * `outer_vars` - Map of variable names from outer scopes (name -> declaration location)
/// * `violations` - Vector to accumulate violations
/// * `rule_id` - Rule ID for violation reporting
fn check_scope_for_shadowing(
    node: &Node,
    source: &str,
    outer_vars: &HashMap<String, (usize, usize)>,
    violations: &mut Vec<RuleViolation>,
    rule_id: &str,
) {
    // Build a new scope with variables declared at this level
    let mut current_scope = outer_vars.clone();

    // Scan for declarations at this level
    match node.kind() {
        "translation_unit" => {
            // File scope - collect all global declarations
            let mut global_vars = HashMap::new();
            collect_declarations_in_node(node, source, &mut global_vars);

            // Check children with global scope
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "function_definition" {
                        check_scope_for_shadowing(
                            &child,
                            source,
                            &global_vars,
                            violations,
                            rule_id,
                        );
                    }
                }
            }
        }
        "function_definition" => {
            // Function scope - collect parameters and check body
            let params = extract_function_parameters(node, source);
            for (param_name, line, col) in params {
                if let Some((outer_line, outer_col)) = outer_vars.get(&param_name) {
                    violations.push(RuleViolation {
                        rule_id: rule_id.to_string(),
                        severity: Severity::Low,
                        message: format!(
                            "Function parameter '{}' shadows variable from outer scope (line {}:{})",
                            param_name, outer_line, outer_col
                        ),
                        file_path: String::new(),
                        line,
                        column: col,
                        suggestion: Some(format!(
                            "Rename parameter '{}' to avoid shadowing outer scope variable",
                            param_name
                        )),
                        ..Default::default()
                    });
                }
                current_scope.insert(param_name, (line, col));
            }

            // Check function body
            if let Some(body) = find_compound_statement(node) {
                check_scope_for_shadowing(&body, source, &current_scope, violations, rule_id);
            }
        }
        "compound_statement" => {
            // Block scope - collect declarations in this block
            let mut block_vars = HashMap::new();

            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    // Check for variable declarations
                    if child.kind() == "declaration" {
                        let decls = extract_declarations(&child, source);
                        for (var_name, line, col) in decls {
                            // Check if this shadows an outer variable
                            if let Some((outer_line, outer_col)) = outer_vars.get(&var_name) {
                                violations.push(RuleViolation {
                                    rule_id: rule_id.to_string(),
                                    severity: Severity::Low,
                                    message: format!(
                                        "Variable '{}' shadows variable from outer scope (line {}:{})",
                                        var_name, outer_line, outer_col
                                    ),
                                    file_path: String::new(),
                                    line,
                                    column: col,
                                    suggestion: Some(format!(
                                        "Rename variable '{}' to avoid shadowing",
                                        var_name
                                    )),
                                    ..Default::default()
                                });
                            }
                            block_vars.insert(var_name, (line, col));
                        }
                    }
                }
            }

            // Merge block variables into current scope
            current_scope.extend(block_vars);

            // Recursively check nested scopes
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    match child.kind() {
                        "compound_statement" | "for_statement" | "while_statement"
                        | "do_statement" | "if_statement" | "switch_statement" => {
                            check_scope_for_shadowing(
                                &child,
                                source,
                                &current_scope,
                                violations,
                                rule_id,
                            );
                        }
                        _ => {}
                    }
                }
            }
        }
        "for_statement" => {
            // For loop - check initializer for declarations
            let mut loop_scope = current_scope.clone();

            // Check for loop variable declarations in initializer
            if let Some(init) = node.child_by_field_name("initializer") {
                if init.kind() == "declaration" {
                    let decls = extract_declarations(&init, source);
                    for (var_name, line, col) in decls {
                        if let Some((outer_line, outer_col)) = outer_vars.get(&var_name) {
                            violations.push(RuleViolation {
                                rule_id: rule_id.to_string(),
                                severity: Severity::Low,
                                message: format!(
                                    "Loop variable '{}' shadows variable from outer scope (line {}:{})",
                                    var_name, outer_line, outer_col
                                ),
                                file_path: String::new(),
                                line,
                                column: col,
                                suggestion: Some(format!(
                                    "Rename loop variable '{}' to avoid shadowing",
                                    var_name
                                )),
                                ..Default::default()
                            });
                        }
                        loop_scope.insert(var_name, (line, col));
                    }
                }
            }

            // Check loop body
            if let Some(body) = node.child_by_field_name("body") {
                check_scope_for_shadowing(&body, source, &loop_scope, violations, rule_id);
            }
        }
        "while_statement" | "do_statement" => {
            // While/do-while loop - check body
            if let Some(body) = node.child_by_field_name("body") {
                check_scope_for_shadowing(&body, source, &current_scope, violations, rule_id);
            }
        }
        "if_statement" => {
            // If statement - check consequence and alternative
            if let Some(consequence) = node.child_by_field_name("consequence") {
                check_scope_for_shadowing(
                    &consequence,
                    source,
                    &current_scope,
                    violations,
                    rule_id,
                );
            }
            if let Some(alternative) = node.child_by_field_name("alternative") {
                check_scope_for_shadowing(
                    &alternative,
                    source,
                    &current_scope,
                    violations,
                    rule_id,
                );
            }
        }
        "switch_statement" => {
            // Switch statement - check body
            if let Some(body) = node.child_by_field_name("body") {
                check_scope_for_shadowing(&body, source, &current_scope, violations, rule_id);
            }
        }
        _ => {
            // For other nodes, recursively check children
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    check_scope_for_shadowing(&child, source, &current_scope, violations, rule_id);
                }
            }
        }
    }
}

/// Collect all variable declarations in a node (non-recursive)
fn collect_declarations_in_node(
    node: &Node,
    source: &str,
    vars: &mut HashMap<String, (usize, usize)>,
) {
    for i in 0..node.child_count() {
        if let Some(child) = node.child(i) {
            if child.kind() == "declaration" {
                let decls = extract_declarations(&child, source);
                for (var_name, line, col) in decls {
                    vars.insert(var_name, (line, col));
                }
            }
        }
    }
}

/// Extract variable names from a declaration node
///
/// Returns: Vec<(variable_name, line_number, column_number)>
fn extract_declarations(decl_node: &Node, source: &str) -> Vec<(String, usize, usize)> {
    let mut declarations = Vec::new();

    // Look for declarators in the declaration
    for i in 0..decl_node.child_count() {
        if let Some(child) = decl_node.child(i) {
            match child.kind() {
                "init_declarator" => {
                    if let Some(declarator) = child.child_by_field_name("declarator") {
                        let var_name =
                            ast_utils::get_identifier_from_declarator(&declarator, source);
                        if !var_name.is_empty() {
                            let pos = declarator.start_position();
                            declarations.push((var_name, pos.row + 1, pos.column + 1));
                        }
                    }
                }
                "pointer_declarator" | "array_declarator" | "identifier" => {
                    let var_name = ast_utils::get_identifier_from_declarator(&child, source);
                    if !var_name.is_empty() {
                        let pos = child.start_position();
                        declarations.push((var_name, pos.row + 1, pos.column + 1));
                    }
                }
                _ => {}
            }
        }
    }

    declarations
}

/// Extract function parameters with their positions
///
/// Returns: Vec<(parameter_name, line_number, column_number)>
fn extract_function_parameters(func_node: &Node, source: &str) -> Vec<(String, usize, usize)> {
    let mut parameters = Vec::new();

    // Find the function_declarator
    for i in 0..func_node.child_count() {
        if let Some(child) = func_node.child(i) {
            if child.kind() == "function_declarator" {
                // Find parameter_list
                for j in 0..child.child_count() {
                    if let Some(param_list) = child.child(j) {
                        if param_list.kind() == "parameter_list" {
                            // Extract each parameter
                            for k in 0..param_list.child_count() {
                                if let Some(param) = param_list.child(k) {
                                    if param.kind() == "parameter_declaration" {
                                        // Find the declarator in the parameter
                                        for m in 0..param.child_count() {
                                            if let Some(declarator) = param.child(m) {
                                                if matches!(
                                                    declarator.kind(),
                                                    "identifier"
                                                        | "pointer_declarator"
                                                        | "array_declarator"
                                                        | "function_declarator"
                                                ) {
                                                    let param_name =
                                                        ast_utils::get_identifier_from_declarator(
                                                            &declarator,
                                                            source,
                                                        );
                                                    if !param_name.is_empty() {
                                                        let pos = declarator.start_position();
                                                        parameters.push((
                                                            param_name,
                                                            pos.row + 1,
                                                            pos.column + 1,
                                                        ));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    parameters
}

/// Find the compound_statement (body) of a function
fn find_compound_statement<'a>(func_node: &Node<'a>) -> Option<Node<'a>> {
    for i in 0..func_node.child_count() {
        if let Some(child) = func_node.child(i) {
            if child.kind() == "compound_statement" {
                return Some(child);
            }
        }
    }
    None
}
