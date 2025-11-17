use anyhow::{Context, Result};
use std::fs;
use tree_sitter::{Parser, Tree};

pub struct CParser {
    parser: Parser,
}

impl CParser {
    pub fn new() -> Result<Self> {
        let mut parser = Parser::new();
        parser
            .set_language(&tree_sitter_c::language())
            .context("Failed to set C language for parser")?;

        Ok(Self { parser })
    }

    pub fn parse_file(&mut self, file_path: &str) -> Result<(Tree, String)> {
        let source = fs::read_to_string(file_path)
            .with_context(|| format!("Failed to read file: {}", file_path))?;

        let tree = self
            .parser
            .parse(&source, None)
            .with_context(|| format!("Failed to parse file: {}", file_path))?;

        Ok((tree, source))
    }

    pub fn parse_source(&mut self, source: &str) -> Result<Tree> {
        self.parser
            .parse(source, None)
            .context("Failed to parse source code")
    }
}

impl Default for CParser {
    fn default() -> Self {
        Self::new().expect("Failed to create C parser")
    }
}
