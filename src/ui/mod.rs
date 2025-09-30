use anyhow::{Context, Result};
use crossterm::{
    event::{self, DisableMouseCapture, EnableMouseCapture, Event, KeyCode},
    execute,
    terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
    tty::IsTty,
};
use ratatui::{
    backend::{Backend, CrosstermBackend},
    layout::{Alignment, Constraint, Direction, Layout},
    style::{Color, Modifier, Style},
    text::{Line, Span},
    widgets::{Block, Borders, List, ListItem, ListState, Paragraph},
    Frame, Terminal,
};
use std::io;

use crate::manifest::RuleManifest;
use crate::rules::{RuleRegistry, RuleViolation};

pub struct TerminalUI {
    repo_path: String,
    manifest: RuleManifest,
    registry: RuleRegistry,
    violations: Vec<RuleViolation>,
    selected_violation: ListState,
}

impl TerminalUI {
    pub fn new(repo_path: &str, manifest: RuleManifest) -> Result<Self> {
        let registry = RuleRegistry::new();
        let mut selected_violation = ListState::default();
        selected_violation.select(Some(0));

        Ok(Self {
            repo_path: repo_path.to_string(),
            manifest,
            registry,
            violations: Vec::new(),
            selected_violation,
        })
    }

    pub fn run(&mut self) -> Result<()> {
        // Check if stdout is a terminal before attempting to use interactive mode
        if !io::stdout().is_tty() {
            return Err(anyhow::anyhow!(
                "Interactive mode requires a terminal. Please run without --interactive flag or from a proper terminal."
            ));
        }

        enable_raw_mode().context("Failed to enable raw mode - ensure you're running in a proper terminal")?;
        let mut stdout = io::stdout();
        execute!(stdout, EnterAlternateScreen, EnableMouseCapture)
            .context("Failed to setup terminal - ensure you're running in a proper terminal")?;
        let backend = CrosstermBackend::new(stdout);
        let mut terminal = Terminal::new(backend)
            .context("Failed to initialize terminal backend")?;

        let result = self.run_app(&mut terminal);

        // Always try to restore terminal state, even if there was an error
        let _ = disable_raw_mode();
        let _ = execute!(
            terminal.backend_mut(),
            LeaveAlternateScreen,
            DisableMouseCapture
        );
        let _ = terminal.show_cursor();

        result
    }

    fn run_app<B: Backend>(&mut self, terminal: &mut Terminal<B>) -> Result<()> {
        loop {
            terminal.draw(|f| self.ui(f))?;

            if let Event::Key(key) = event::read()? {
                match key.code {
                    KeyCode::Char('q') => return Ok(()),
                    KeyCode::Down => {
                        let i = match self.selected_violation.selected() {
                            Some(i) => {
                                if i >= self.violations.len().saturating_sub(1) {
                                    0
                                } else {
                                    i + 1
                                }
                            }
                            None => 0,
                        };
                        self.selected_violation.select(Some(i));
                    }
                    KeyCode::Up => {
                        let i = match self.selected_violation.selected() {
                            Some(i) => {
                                if i == 0 {
                                    self.violations.len().saturating_sub(1)
                                } else {
                                    i - 1
                                }
                            }
                            None => 0,
                        };
                        self.selected_violation.select(Some(i));
                    }
                    KeyCode::Char('s') => {
                        // Trigger scan
                        self.scan_repository()?;
                    }
                    _ => {}
                }
            }
        }
    }

    fn ui(&mut self, f: &mut Frame) {
        let chunks = Layout::default()
            .direction(Direction::Vertical)
            .margin(1)
            .constraints([
                Constraint::Length(3),
                Constraint::Min(10),
                Constraint::Length(3),
            ])
            .split(f.area());

        // Header
        let header = Paragraph::new(vec![
            Line::from(vec![
                Span::styled("SqC - Software Code Quality", Style::default().fg(Color::Green).add_modifier(Modifier::BOLD)),
            ]),
            Line::from(vec![
                Span::raw("Repository: "),
                Span::styled(&self.repo_path, Style::default().fg(Color::Yellow)),
            ]),
        ])
        .block(Block::default().borders(Borders::ALL).title("CERT C Compliance Checker"));

        f.render_widget(header, chunks[0]);

        // Violations list
        let violations: Vec<ListItem> = self
            .violations
            .iter()
            .map(|v| {
                let severity_color = match v.severity {
                    crate::manifest::Severity::Critical => Color::Red,
                    crate::manifest::Severity::High => Color::LightRed,
                    crate::manifest::Severity::Medium => Color::Yellow,
                    crate::manifest::Severity::Low => Color::Blue,
                };

                ListItem::new(vec![Line::from(vec![
                    Span::styled(&v.rule_id, Style::default().fg(severity_color).add_modifier(Modifier::BOLD)),
                    Span::raw(" - "),
                    Span::raw(&v.message),
                    Span::raw(" ("),
                    Span::styled(format!("{}:{}", v.line, v.column), Style::default().fg(Color::Cyan)),
                    Span::raw(")"),
                ])])
            })
            .collect();

        let violations_list = List::new(violations)
            .block(Block::default().borders(Borders::ALL).title("Violations"))
            .highlight_style(Style::default().add_modifier(Modifier::REVERSED))
            .highlight_symbol(">> ");

        f.render_stateful_widget(violations_list, chunks[1], &mut self.selected_violation);

        // Footer
        let footer = Paragraph::new("Press 's' to scan, 'q' to quit, ↑/↓ to navigate")
            .style(Style::default().fg(Color::White))
            .alignment(Alignment::Center)
            .block(Block::default().borders(Borders::ALL));

        f.render_widget(footer, chunks[2]);
    }

    fn scan_repository(&mut self) -> Result<()> {
        use crate::git::ProjectSource;
        use crate::parser::CParser;

        self.violations.clear();

        let project_source = ProjectSource::open(&self.repo_path)?;
        let c_files = project_source.get_c_files()?;
        let mut parser = CParser::new()?;

        for file_path in c_files {
            if let Ok((tree, source)) = parser.parse_file(&file_path) {
                let root_node = tree.root_node();

                for (rule_id, rule_config) in self.manifest.enabled_rules() {
                    if let Some(rule) = self.registry.get_rule(rule_id) {
                        let mut file_violations = rule.check(&root_node, &source);
                        for violation in &mut file_violations {
                            violation.file_path = file_path.clone();
                            violation.severity = rule_config.severity.clone();
                        }
                        self.violations.extend(file_violations);
                    }
                }
            }
        }

        if !self.violations.is_empty() {
            self.selected_violation.select(Some(0));
        }

        Ok(())
    }
}