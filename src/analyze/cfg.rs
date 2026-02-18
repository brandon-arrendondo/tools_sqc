//! Control-flow graph construction from tree-sitter AST.
//!
//! Builds a lightweight CFG from C function bodies. Each basic block contains
//! a sequence of statements with a single entry and single exit. Edges represent
//! control flow between blocks (fallthrough, branches, back edges, returns).

use std::collections::HashMap;
use tree_sitter::Node;

/// Unique identifier for a basic block within a function CFG.
pub type BlockId = usize;

/// A basic block: a straight-line sequence of statements.
#[derive(Debug, Clone)]
pub struct BasicBlock {
    pub id: BlockId,
    /// Byte ranges of statements in this block (start, end).
    pub statements: Vec<(usize, usize)>,
    /// Overall byte range of this block.
    pub byte_range: (usize, usize),
}

/// Edge types in the control-flow graph.
#[derive(Debug, Clone, PartialEq, Eq)]
pub enum CfgEdge {
    /// Sequential fallthrough to the next block.
    Fallthrough,
    /// True branch of an if/while/for condition.
    TrueBranch,
    /// False branch of an if/while/for condition.
    FalseBranch,
    /// Back edge from loop body to loop header.
    BackEdge,
    /// Return from function (edge to exit block).
    Return,
    /// Break out of a loop.
    Break,
    /// Continue to loop header.
    Continue,
}

/// A control-flow graph for a single function.
#[derive(Debug)]
pub struct FunctionCfg {
    pub blocks: Vec<BasicBlock>,
    pub edges: Vec<(BlockId, BlockId, CfgEdge)>,
    pub entry: BlockId,
    pub exits: Vec<BlockId>,
    /// Source code for the function (for extracting text).
    function_start_byte: usize,
}

impl FunctionCfg {
    /// Get successors of a block.
    pub fn successors(&self, block_id: BlockId) -> Vec<(BlockId, &CfgEdge)> {
        self.edges
            .iter()
            .filter(|(from, _, _)| *from == block_id)
            .map(|(_, to, edge)| (*to, edge))
            .collect()
    }

    /// Get predecessors of a block.
    pub fn predecessors(&self, block_id: BlockId) -> Vec<(BlockId, &CfgEdge)> {
        self.edges
            .iter()
            .filter(|(_, to, _)| *to == block_id)
            .map(|(from, _, edge)| (*from, edge))
            .collect()
    }

    /// Get the number of blocks.
    pub fn block_count(&self) -> usize {
        self.blocks.len()
    }

    /// Get a block by ID.
    pub fn get_block(&self, id: BlockId) -> Option<&BasicBlock> {
        self.blocks.get(id)
    }
}

/// Builder for constructing a CFG from a function's compound_statement body.
struct CfgBuilder {
    blocks: Vec<BasicBlock>,
    edges: Vec<(BlockId, BlockId, CfgEdge)>,
    current_block: BlockId,
    /// Stack of (loop_header, loop_exit) for break/continue targets.
    loop_stack: Vec<(BlockId, BlockId)>,
    function_start_byte: usize,
}

impl CfgBuilder {
    fn new(function_start_byte: usize) -> Self {
        let entry_block = BasicBlock {
            id: 0,
            statements: Vec::new(),
            byte_range: (0, 0),
        };
        CfgBuilder {
            blocks: vec![entry_block],
            edges: Vec::new(),
            current_block: 0,
            loop_stack: Vec::new(),
            function_start_byte,
        }
    }

    fn new_block(&mut self) -> BlockId {
        let id = self.blocks.len();
        self.blocks.push(BasicBlock {
            id,
            statements: Vec::new(),
            byte_range: (0, 0),
        });
        id
    }

    fn add_edge(&mut self, from: BlockId, to: BlockId, kind: CfgEdge) {
        // Avoid duplicate edges
        if !self
            .edges
            .iter()
            .any(|(f, t, k)| *f == from && *t == to && *k == kind)
        {
            self.edges.push((from, to, kind));
        }
    }

    fn add_statement(&mut self, start: usize, end: usize) {
        if let Some(block) = self.blocks.get_mut(self.current_block) {
            block.statements.push((start, end));
            if block.byte_range.0 == 0 || start < block.byte_range.0 {
                block.byte_range.0 = start;
            }
            if end > block.byte_range.1 {
                block.byte_range.1 = end;
            }
        }
    }

    fn build_from_compound_statement<'a>(&mut self, node: &Node<'a>, source: &str) {
        for i in 0..node.child_count() {
            if let Some(child) = node.child(i) {
                match child.kind() {
                    "{" | "}" => continue,
                    _ => self.process_statement(&child, source),
                }
            }
        }
    }

    fn process_statement<'a>(&mut self, node: &Node<'a>, source: &str) {
        match node.kind() {
            "if_statement" => self.process_if(node, source),
            "while_statement" => self.process_while(node, source),
            "for_statement" => self.process_for(node, source),
            "do_statement" => self.process_do_while(node, source),
            "switch_statement" => {
                // Treat switch as a single statement for now
                self.add_statement(node.start_byte(), node.end_byte());
            }
            "return_statement" => {
                self.add_statement(node.start_byte(), node.end_byte());
                let exit_block = self.new_block();
                self.add_edge(self.current_block, exit_block, CfgEdge::Return);
                self.current_block = self.new_block(); // Unreachable block after return
            }
            "break_statement" => {
                self.add_statement(node.start_byte(), node.end_byte());
                if let Some(&(_, loop_exit)) = self.loop_stack.last() {
                    self.add_edge(self.current_block, loop_exit, CfgEdge::Break);
                }
                self.current_block = self.new_block(); // Unreachable block after break
            }
            "continue_statement" => {
                self.add_statement(node.start_byte(), node.end_byte());
                if let Some(&(loop_header, _)) = self.loop_stack.last() {
                    self.add_edge(self.current_block, loop_header, CfgEdge::Continue);
                }
                self.current_block = self.new_block(); // Unreachable block after continue
            }
            "goto_statement" => {
                // Treat goto as a terminal statement (conservative)
                self.add_statement(node.start_byte(), node.end_byte());
                self.current_block = self.new_block();
            }
            "compound_statement" => {
                self.build_from_compound_statement(node, source);
            }
            "labeled_statement" => {
                // Process the labeled statement's body
                for i in 0..node.child_count() {
                    if let Some(child) = node.child(i) {
                        if child.kind() != ":" && child.kind() != "identifier" {
                            self.process_statement(&child, source);
                        }
                    }
                }
            }
            _ => {
                // Regular statement: add to current block
                self.add_statement(node.start_byte(), node.end_byte());
            }
        }
    }

    fn process_if<'a>(&mut self, node: &Node<'a>, source: &str) {
        // Add the condition to the current block
        if let Some(condition) = node.child_by_field_name("condition") {
            self.add_statement(condition.start_byte(), condition.end_byte());
        }

        let condition_block = self.current_block;
        let then_block = self.new_block();
        let join_block = self.new_block();

        // True branch
        self.add_edge(condition_block, then_block, CfgEdge::TrueBranch);
        self.current_block = then_block;

        if let Some(consequence) = node.child_by_field_name("consequence") {
            self.process_statement(&consequence, source);
        }
        self.add_edge(self.current_block, join_block, CfgEdge::Fallthrough);

        // False branch
        if let Some(alternative) = node.child_by_field_name("alternative") {
            let else_block = self.new_block();
            self.add_edge(condition_block, else_block, CfgEdge::FalseBranch);
            self.current_block = else_block;

            // else_clause has a child that is the actual statement
            for i in 0..alternative.child_count() {
                if let Some(child) = alternative.child(i) {
                    if child.kind() != "else" {
                        self.process_statement(&child, source);
                    }
                }
            }
            self.add_edge(self.current_block, join_block, CfgEdge::Fallthrough);
        } else {
            self.add_edge(condition_block, join_block, CfgEdge::FalseBranch);
        }

        self.current_block = join_block;
    }

    fn process_while<'a>(&mut self, node: &Node<'a>, source: &str) {
        let header_block = self.new_block();
        self.add_edge(self.current_block, header_block, CfgEdge::Fallthrough);

        // Add condition to header block
        self.current_block = header_block;
        if let Some(condition) = node.child_by_field_name("condition") {
            self.add_statement(condition.start_byte(), condition.end_byte());
        }

        let body_block = self.new_block();
        let exit_block = self.new_block();

        self.add_edge(header_block, body_block, CfgEdge::TrueBranch);
        self.add_edge(header_block, exit_block, CfgEdge::FalseBranch);

        // Process body
        self.loop_stack.push((header_block, exit_block));
        self.current_block = body_block;
        if let Some(body) = node.child_by_field_name("body") {
            self.process_statement(&body, source);
        }
        self.add_edge(self.current_block, header_block, CfgEdge::BackEdge);
        self.loop_stack.pop();

        self.current_block = exit_block;
    }

    fn process_for<'a>(&mut self, node: &Node<'a>, source: &str) {
        // Initializer in current block
        if let Some(initializer) = node.child_by_field_name("initializer") {
            self.add_statement(initializer.start_byte(), initializer.end_byte());
        }

        let header_block = self.new_block();
        self.add_edge(self.current_block, header_block, CfgEdge::Fallthrough);

        // Condition in header block
        self.current_block = header_block;
        if let Some(condition) = node.child_by_field_name("condition") {
            self.add_statement(condition.start_byte(), condition.end_byte());
        }

        let body_block = self.new_block();
        let update_block = self.new_block();
        let exit_block = self.new_block();

        self.add_edge(header_block, body_block, CfgEdge::TrueBranch);
        self.add_edge(header_block, exit_block, CfgEdge::FalseBranch);

        // Process body
        self.loop_stack.push((update_block, exit_block));
        self.current_block = body_block;
        if let Some(body) = node.child_by_field_name("body") {
            self.process_statement(&body, source);
        }
        self.add_edge(self.current_block, update_block, CfgEdge::Fallthrough);
        self.loop_stack.pop();

        // Update expression
        self.current_block = update_block;
        if let Some(update) = node.child_by_field_name("update") {
            self.add_statement(update.start_byte(), update.end_byte());
        }
        self.add_edge(update_block, header_block, CfgEdge::BackEdge);

        self.current_block = exit_block;
    }

    fn process_do_while<'a>(&mut self, node: &Node<'a>, source: &str) {
        let body_block = self.new_block();
        self.add_edge(self.current_block, body_block, CfgEdge::Fallthrough);

        let exit_block = self.new_block();

        // Process body first (do-while executes body before checking condition)
        self.loop_stack.push((body_block, exit_block));
        self.current_block = body_block;
        if let Some(body) = node.child_by_field_name("body") {
            self.process_statement(&body, source);
        }
        self.loop_stack.pop();

        // Condition block
        let cond_block = self.new_block();
        self.add_edge(self.current_block, cond_block, CfgEdge::Fallthrough);
        self.current_block = cond_block;

        if let Some(condition) = node.child_by_field_name("condition") {
            self.add_statement(condition.start_byte(), condition.end_byte());
        }

        self.add_edge(cond_block, body_block, CfgEdge::BackEdge);
        self.add_edge(cond_block, exit_block, CfgEdge::FalseBranch);

        self.current_block = exit_block;
    }

    fn build(self) -> FunctionCfg {
        // Find exit blocks (blocks with Return edges, or the last block if it has no successors)
        let mut exits: Vec<BlockId> = self
            .edges
            .iter()
            .filter(|(_, _, kind)| *kind == CfgEdge::Return)
            .map(|(from, _, _)| *from)
            .collect();

        // Also include terminal blocks (those in Return edges as targets)
        let return_targets: Vec<BlockId> = self
            .edges
            .iter()
            .filter(|(_, _, kind)| *kind == CfgEdge::Return)
            .map(|(_, to, _)| *to)
            .collect();
        exits.extend(return_targets);

        // If no explicit returns, the last block is an implicit exit
        if exits.is_empty() && !self.blocks.is_empty() {
            exits.push(self.blocks.len() - 1);
        }

        exits.sort();
        exits.dedup();

        FunctionCfg {
            blocks: self.blocks,
            edges: self.edges,
            entry: 0,
            exits,
            function_start_byte: self.function_start_byte,
        }
    }
}

/// Build a CFG from a function_definition node.
/// Returns None if the node is not a function_definition or has no body.
pub fn build_function_cfg(func_node: &Node, source: &str) -> Option<FunctionCfg> {
    if func_node.kind() != "function_definition" {
        return None;
    }

    let body = func_node.child_by_field_name("body")?;
    if body.kind() != "compound_statement" {
        return None;
    }

    let mut builder = CfgBuilder::new(func_node.start_byte());
    builder.build_from_compound_statement(&body, source);
    Some(builder.build())
}

/// Extract the function name from a function_definition node.
pub fn get_function_name<'a>(func_node: &Node<'a>, source: &'a str) -> Option<&'a str> {
    let declarator = func_node.child_by_field_name("declarator")?;
    extract_name_from_declarator(&declarator, source)
}

fn extract_name_from_declarator<'a>(node: &Node<'a>, source: &'a str) -> Option<&'a str> {
    match node.kind() {
        "identifier" => node.utf8_text(source.as_bytes()).ok(),
        "function_declarator" | "pointer_declarator" => {
            let inner = node.child_by_field_name("declarator")?;
            extract_name_from_declarator(&inner, source)
        }
        _ => {
            for i in 0..node.child_count() {
                if let Some(child) = node.child(i) {
                    if child.kind() == "identifier" {
                        return child.utf8_text(source.as_bytes()).ok();
                    }
                }
            }
            None
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn parse_and_build_cfg(code: &str) -> Option<FunctionCfg> {
        let mut parser = tree_sitter::Parser::new();
        parser.set_language(&tree_sitter_c::language()).unwrap();
        let tree = parser.parse(code, None).unwrap();
        let root = tree.root_node();

        // Find the function_definition
        for i in 0..root.child_count() {
            if let Some(child) = root.child(i) {
                if child.kind() == "function_definition" {
                    return build_function_cfg(&child, code);
                }
            }
        }
        None
    }

    #[test]
    fn test_simple_function() {
        let code = r#"
        void foo() {
            int x = 1;
            int y = 2;
        }
        "#;
        let cfg = parse_and_build_cfg(code).unwrap();
        assert!(cfg.block_count() >= 1);
        assert_eq!(cfg.entry, 0);
    }

    #[test]
    fn test_if_else() {
        let code = r#"
        void foo(int x) {
            if (x > 0) {
                x = 1;
            } else {
                x = 2;
            }
            x = 3;
        }
        "#;
        let cfg = parse_and_build_cfg(code).unwrap();
        // Should have: entry, then-block, else-block, join-block (minimum)
        assert!(cfg.block_count() >= 4);
        // Should have true and false branch edges
        let has_true = cfg.edges.iter().any(|(_, _, e)| *e == CfgEdge::TrueBranch);
        let has_false = cfg.edges.iter().any(|(_, _, e)| *e == CfgEdge::FalseBranch);
        assert!(has_true);
        assert!(has_false);
    }

    #[test]
    fn test_while_loop() {
        let code = r#"
        void foo(int n) {
            int i = 0;
            while (i < n) {
                i++;
            }
        }
        "#;
        let cfg = parse_and_build_cfg(code).unwrap();
        // Should have a back edge
        let has_back = cfg.edges.iter().any(|(_, _, e)| *e == CfgEdge::BackEdge);
        assert!(has_back);
    }

    #[test]
    fn test_for_loop() {
        let code = r#"
        void foo() {
            for (int i = 0; i < 10; i++) {
                int x = i;
            }
        }
        "#;
        let cfg = parse_and_build_cfg(code).unwrap();
        let has_back = cfg.edges.iter().any(|(_, _, e)| *e == CfgEdge::BackEdge);
        assert!(has_back);
    }

    #[test]
    fn test_return_creates_exit() {
        let code = r#"
        int foo(int x) {
            if (x < 0) {
                return -1;
            }
            return x;
        }
        "#;
        let cfg = parse_and_build_cfg(code).unwrap();
        let return_count = cfg
            .edges
            .iter()
            .filter(|(_, _, e)| *e == CfgEdge::Return)
            .count();
        assert!(return_count >= 2);
    }

    #[test]
    fn test_break_continue() {
        let code = r#"
        void foo(int n) {
            for (int i = 0; i < n; i++) {
                if (i == 5) break;
                if (i == 3) continue;
            }
        }
        "#;
        let cfg = parse_and_build_cfg(code).unwrap();
        let has_break = cfg.edges.iter().any(|(_, _, e)| *e == CfgEdge::Break);
        let has_continue = cfg.edges.iter().any(|(_, _, e)| *e == CfgEdge::Continue);
        assert!(has_break);
        assert!(has_continue);
    }
}
