use anyhow::{Context, Result};
use crossterm::{
    event::{self, DisableMouseCapture, EnableMouseCapture, Event, KeyCode, MouseEventKind},
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
use std::fs;
use std::collections::HashSet;
use std::path::PathBuf;
use sha2::{Sha256, Digest};

use crate::manifest::RuleManifest;
use crate::rules::{RuleRegistry, RuleViolation};

pub struct TerminalUI {
    repo_path: String,
    manifest: RuleManifest,
    registry: RuleRegistry,
    violations: Vec<RuleViolation>,
    selected_violation: ListState,
    checked_violations: HashSet<usize>,
    show_save_dialog: bool,
    save_filename: String,
    show_file_preview: bool,
    preview_focused: bool,
    preview_scroll_offset: usize,
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
            checked_violations: HashSet::new(),
            show_save_dialog: false,
            save_filename: String::from("violations.xlsx"),
            show_file_preview: true, // Default to showing preview
            preview_focused: false, // Default focus on violations list
            preview_scroll_offset: 0,
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

            match event::read()? {
                Event::Key(key) => {
                if self.show_save_dialog {
                    match key.code {
                        KeyCode::Enter => {
                            // Save the file (CSV or Excel based on extension)
                            let path = PathBuf::from(&self.save_filename);
                            let _ = self.export_violations(&path); // Ignore errors for now
                            self.show_save_dialog = false;
                        }
                        KeyCode::Esc => {
                            // Cancel the save dialog
                            self.show_save_dialog = false;
                        }
                        KeyCode::Backspace => {
                            // Remove last character from filename
                            self.save_filename.pop();
                        }
                        KeyCode::Char(c) => {
                            // Add character to filename
                            self.save_filename.push(c);
                        }
                        _ => {}
                    }
                } else {
                    match key.code {
                        KeyCode::Char('q') | KeyCode::Esc => return Ok(()),
                        KeyCode::Down => {
                            if self.preview_focused && self.show_file_preview {
                                self.preview_scroll_down();
                            } else {
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
                                // Reset preview scroll when changing violations
                                self.preview_scroll_offset = 0;
                            }
                        }
                        KeyCode::Up => {
                            if self.preview_focused && self.show_file_preview {
                                self.preview_scroll_up();
                            } else {
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
                                // Reset preview scroll when changing violations
                                self.preview_scroll_offset = 0;
                            }
                        }
                        KeyCode::Char('s') => {
                            // Trigger scan
                            self.scan_repository()?;
                        }
                        KeyCode::Char(' ') => {
                            // Toggle checkbox for selected violation
                            if let Some(index) = self.selected_violation.selected() {
                                if self.checked_violations.contains(&index) {
                                    self.checked_violations.remove(&index);
                                } else {
                                    self.checked_violations.insert(index);
                                }
                            }
                        }
                        KeyCode::Char('e') => {
                            // Show save dialog for exporting selected violations to CSV
                            if !self.checked_violations.is_empty() {
                                self.show_save_dialog = true;
                            }
                        }
                        KeyCode::Char('a') => {
                            // Select all violations
                            self.checked_violations = (0..self.violations.len()).collect();
                        }
                        KeyCode::Char('n') => {
                            // Deselect all violations
                            self.checked_violations.clear();
                        }
                        KeyCode::Char('p') => {
                            // Toggle file preview
                            self.show_file_preview = !self.show_file_preview;
                            if !self.show_file_preview {
                                self.preview_focused = false; // Reset focus if preview is hidden
                            }
                        }
                        KeyCode::Tab => {
                            // Switch focus between violations list and file preview
                            if self.show_file_preview {
                                self.preview_focused = !self.preview_focused;
                            }
                        }
                        _ => {}
                    }
                }
                }
                Event::Mouse(mouse_event) => {
                    // Only handle mouse events when not in save dialog
                    if !self.show_save_dialog {
                        match mouse_event.kind {
                            MouseEventKind::ScrollUp => {
                                if self.preview_focused && self.show_file_preview {
                                    self.preview_scroll_up();
                                } else {
                                    self.scroll_up();
                                }
                            }
                            MouseEventKind::ScrollDown => {
                                if self.preview_focused && self.show_file_preview {
                                    self.preview_scroll_down();
                                } else {
                                    self.scroll_down();
                                }
                            }
                            _ => {}
                        }
                    }
                }
                _ => {}
            }
        }
    }

    fn ui(&mut self, f: &mut Frame) {
        if self.show_save_dialog {
            self.render_save_dialog(f);
            return;
        }

        let chunks = if self.show_file_preview {
            // Split vertically: header, violations, preview, footer
            Layout::default()
                .direction(Direction::Vertical)
                .margin(1)
                .constraints([
                    Constraint::Length(3),     // Header
                    Constraint::Percentage(50), // Violations list
                    Constraint::Percentage(50), // File preview
                    Constraint::Length(3),     // Footer
                ])
                .split(f.area())
        } else {
            // Original layout: header, violations, footer
            Layout::default()
                .direction(Direction::Vertical)
                .margin(1)
                .constraints([
                    Constraint::Length(3),     // Header
                    Constraint::Min(10),       // Violations list
                    Constraint::Length(3),     // Footer
                ])
                .split(f.area())
        };

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
            .enumerate()
            .map(|(index, v)| {
                let severity_color = match v.severity {
                    crate::manifest::Severity::Critical => Color::Red,
                    crate::manifest::Severity::High => Color::LightRed,
                    crate::manifest::Severity::Medium => Color::Yellow,
                    crate::manifest::Severity::Low => Color::Blue,
                };

                let checkbox = if self.checked_violations.contains(&index) {
                    "[✓] "
                } else {
                    "[ ] "
                };

                ListItem::new(vec![Line::from(vec![
                    Span::styled(checkbox, Style::default().fg(Color::Green)),
                    Span::styled(&v.rule_id, Style::default().fg(severity_color).add_modifier(Modifier::BOLD)),
                    Span::raw(" - "),
                    Span::raw(&v.message),
                    Span::raw(" ("),
                    Span::styled(format!("{}:{}", v.line, v.column), Style::default().fg(Color::Cyan)),
                    Span::raw(")"),
                ])])
            })
            .collect();

        let violations_title = if !self.preview_focused || !self.show_file_preview {
            "[FOCUSED] Violations"
        } else {
            "Violations - Tab to focus"
        };

        let violations_block = if !self.preview_focused || !self.show_file_preview {
            Block::default()
                .borders(Borders::ALL)
                .border_style(Style::default().fg(Color::Yellow))
                .title(violations_title)
        } else {
            Block::default()
                .borders(Borders::ALL)
                .title(violations_title)
        };

        let violations_list = List::new(violations)
            .block(violations_block)
            .highlight_style(Style::default().add_modifier(Modifier::REVERSED))
            .highlight_symbol(">> ");

        f.render_stateful_widget(violations_list, chunks[1], &mut self.selected_violation);

        // File preview panel (only if enabled)
        if self.show_file_preview {
            self.render_file_preview(f, chunks[2]);
        }

        // Footer
        let footer = Paragraph::new("(s)can, (e)xport, (space)select, (a)ll, (n)one, (p)review, (tab)focus, (q)/Esc quit")
            .style(Style::default().fg(Color::White))
            .alignment(Alignment::Center)
            .block(Block::default().borders(Borders::ALL));

        let footer_index = if self.show_file_preview { 3 } else { 2 };
        f.render_widget(footer, chunks[footer_index]);
    }

    fn render_save_dialog(&mut self, f: &mut Frame) {
        use ratatui::layout::{Rect};

        let area = f.area();
        let popup_area = Rect {
            x: area.width / 4,
            y: area.height / 2 - 3,
            width: area.width / 2,
            height: 6,
        };

        // Clear the popup area
        f.render_widget(
            Block::default()
                .style(Style::default().bg(Color::Black))
                .borders(Borders::ALL)
                .title("Save CSV Export"),
            popup_area,
        );

        let inner_area = Rect {
            x: popup_area.x + 1,
            y: popup_area.y + 1,
            width: popup_area.width - 2,
            height: popup_area.height - 2,
        };

        let dialog_text = vec![
            Line::from("Enter filename (.csv or .xlsx):"),
            Line::from(vec![
                Span::styled(&self.save_filename, Style::default().fg(Color::Yellow)),
                Span::styled("_", Style::default().fg(Color::Green)), // cursor
            ]),
            Line::from(""),
            Line::from("Press Enter to save, Esc to cancel"),
        ];

        let dialog_paragraph = Paragraph::new(dialog_text)
            .style(Style::default().fg(Color::White));

        f.render_widget(dialog_paragraph, inner_area);
    }

    fn render_file_preview(&mut self, f: &mut Frame, area: ratatui::layout::Rect) {
        if let Some(selected_index) = self.selected_violation.selected() {
            if let Some(violation) = self.violations.get(selected_index) {
                // Try to read and display the file content
                match self.get_file_preview(&violation.file_path, violation.line) {
                    Ok(preview_lines) => {
                        // Apply scroll offset
                        let scrolled_lines: Vec<_> = preview_lines
                            .into_iter()
                            .skip(self.preview_scroll_offset)
                            .collect();

                        let title = if self.preview_focused {
                            format!("[FOCUSED] Preview: {} (line {})",
                                std::path::Path::new(&violation.file_path)
                                    .file_name()
                                    .unwrap_or_default()
                                    .to_string_lossy(),
                                violation.line)
                        } else {
                            format!("Preview: {} (line {}) - Tab to focus",
                                std::path::Path::new(&violation.file_path)
                                    .file_name()
                                    .unwrap_or_default()
                                    .to_string_lossy(),
                                violation.line)
                        };

                        let block_style = if self.preview_focused {
                            Block::default()
                                .borders(Borders::ALL)
                                .border_style(Style::default().fg(Color::Yellow))
                                .title(title)
                        } else {
                            Block::default()
                                .borders(Borders::ALL)
                                .title(title)
                        };

                        let preview_paragraph = Paragraph::new(scrolled_lines)
                            .block(block_style)
                            .wrap(ratatui::widgets::Wrap { trim: false });

                        f.render_widget(preview_paragraph, area);
                    }
                    Err(_) => {
                        let error_paragraph = Paragraph::new("Unable to preview file")
                            .block(Block::default()
                                .borders(Borders::ALL)
                                .title("File Preview"))
                            .style(Style::default().fg(Color::Red));

                        f.render_widget(error_paragraph, area);
                    }
                }
            } else {
                // No violation selected
                let empty_paragraph = Paragraph::new("Select a violation to preview file")
                    .block(Block::default()
                        .borders(Borders::ALL)
                        .title("File Preview"))
                    .style(Style::default().fg(Color::Gray));

                f.render_widget(empty_paragraph, area);
            }
        } else {
            // No violations available
            let empty_paragraph = Paragraph::new("No violations to preview")
                .block(Block::default()
                    .borders(Borders::ALL)
                    .title("File Preview"))
                .style(Style::default().fg(Color::Gray));

            f.render_widget(empty_paragraph, area);
        }
    }

    fn get_file_preview(&self, file_path: &str, target_line: usize) -> Result<Vec<Line>> {
        let content = fs::read_to_string(file_path)?;
        let lines: Vec<&str> = content.lines().collect();

        // Show context around the target line (5 lines before and after)
        let context_size = 5;
        let start_line = target_line.saturating_sub(context_size).max(1);
        let end_line = (target_line + context_size).min(lines.len());

        let mut preview_lines = Vec::new();

        for line_num in start_line..=end_line {
            if let Some(line_content) = lines.get(line_num - 1) {
                let line_number_str = format!("{:4} ", line_num);

                if line_num == target_line {
                    // Highlight the target line
                    preview_lines.push(Line::from(vec![
                        Span::styled(line_number_str, Style::default().fg(Color::Yellow).add_modifier(Modifier::BOLD)),
                        Span::styled(">>> ", Style::default().fg(Color::Red).add_modifier(Modifier::BOLD)),
                        Span::styled(line_content.to_string(), Style::default().fg(Color::White).bg(Color::Red)),
                    ]));
                } else {
                    // Regular line
                    preview_lines.push(Line::from(vec![
                        Span::styled(line_number_str, Style::default().fg(Color::Cyan)),
                        Span::styled("    ", Style::default()),
                        Span::styled(line_content.to_string(), Style::default().fg(Color::White)),
                    ]));
                }
            }
        }

        Ok(preview_lines)
    }

    fn scroll_up(&mut self) {
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

    fn scroll_down(&mut self) {
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

    fn preview_scroll_up(&mut self) {
        if self.preview_scroll_offset > 0 {
            self.preview_scroll_offset -= 1;
        }
    }

    fn preview_scroll_down(&mut self) {
        self.preview_scroll_offset += 1;
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

    fn export_violations(&self, path: &PathBuf) -> Result<()> {
        if let Some(extension) = path.extension() {
            match extension.to_str() {
                Some("xlsx") => self.write_violations_to_excel(path),
                Some("csv") => self.write_violations_to_csv(path),
                _ => {
                    // Default to Excel for unknown extensions
                    self.write_violations_to_excel(path)
                }
            }
        } else {
            // No extension, default to Excel
            self.write_violations_to_excel(path)
        }
    }

    fn write_violations_to_csv(&self, path: &PathBuf) -> Result<()> {
        use csv::Writer;

        let mut writer = Writer::from_path(path)?;

        // Write CSV headers
        writer.write_record(&[
            "Title",
            "Description",
            "Work Item Type",
            "State",
            "Severity",
            "Priority"
        ])?;

        // Write selected violations only
        for &index in &self.checked_violations {
            if let Some(violation) = self.violations.get(index) {
                let file_hash = self.calculate_file_hash(&violation.file_path)?;
                let relative_path = self.get_relative_path(&violation.file_path);

                let title = format!("{}:{}:{} version:{}",
                    violation.rule_id, relative_path, violation.line, file_hash);

                let code_snippet = self.get_code_snippet(&violation.file_path, violation.line)?;
                let rule_description = self.get_rule_description(&violation.rule_id);
                let description = format!("{} - {}: {}",
                    violation.rule_id, rule_description, code_snippet);

                writer.write_record(&[
                    &title,
                    &description,
                    "Bug",
                    "Proposed",
                    "1 - Critical",
                    "1"
                ])?;
            }
        }

        writer.flush()?;
        Ok(())
    }

    fn write_violations_to_excel(&self, path: &PathBuf) -> Result<()> {
        use rust_xlsxwriter::{Workbook, Format, Color as XlsxColor};

        let mut workbook = Workbook::new();
        let worksheet = workbook.add_worksheet();

        // Create header format
        let header_format = Format::new()
            .set_bold()
            .set_background_color(XlsxColor::RGB(0xD9D9D9));

        // Write headers
        let headers = [
            "Title",
            "Description",
            "Work Item Type",
            "State",
            "Severity",
            "Priority"
        ];

        for (col, header) in headers.iter().enumerate() {
            worksheet.write_string_with_format(0, col as u16, *header, &header_format)?;
        }

        // Write selected violations
        let mut row = 1;
        for &index in &self.checked_violations {
            if let Some(violation) = self.violations.get(index) {
                let file_hash = self.calculate_file_hash(&violation.file_path)?;
                let relative_path = self.get_relative_path(&violation.file_path);

                let title = format!("{}:{}:{} version:{}",
                    violation.rule_id, relative_path, violation.line, file_hash);

                let code_snippet = self.get_code_snippet(&violation.file_path, violation.line)?;
                let rule_description = self.get_rule_description(&violation.rule_id);
                let description = format!("{} - {}: {}",
                    violation.rule_id, rule_description, code_snippet);

                worksheet.write_string(row, 0, &title)?;
                worksheet.write_string(row, 1, &description)?;
                worksheet.write_string(row, 2, "Bug")?;
                worksheet.write_string(row, 3, "Proposed")?;
                worksheet.write_string(row, 4, "1 - Critical")?;
                worksheet.write_string(row, 5, "1")?;

                row += 1;
            }
        }

        // Auto-fit columns
        worksheet.autofit();

        workbook.save(path)?;
        Ok(())
    }

    fn calculate_file_hash(&self, file_path: &str) -> Result<String> {
        let content = fs::read(file_path)?;
        let mut hasher = Sha256::new();
        hasher.update(&content);
        let result = hasher.finalize();
        Ok(format!("{:x}", result)[..8].to_string())
    }

    fn get_code_snippet(&self, file_path: &str, line_number: usize) -> Result<String> {
        let content = fs::read_to_string(file_path)?;
        let lines: Vec<&str> = content.lines().collect();

        if line_number > 0 && line_number <= lines.len() {
            let line = lines[line_number - 1].trim();
            Ok(line.to_string())
        } else {
            Ok("(line not found)".to_string())
        }
    }

    fn get_rule_description(&self, rule_id: &str) -> String {
        if let Some(rule) = self.registry.get_rule(rule_id) {
            rule.description().to_string()
        } else {
            "Unknown rule".to_string()
        }
    }

    fn get_relative_path(&self, file_path: &str) -> String {
        use std::path::Path;

        let base_path_obj = Path::new(&self.repo_path);
        let file_path_obj = Path::new(file_path);

        if let Ok(relative) = file_path_obj.strip_prefix(base_path_obj) {
            relative.to_string_lossy().to_string()
        } else {
            file_path.split('/').last().unwrap_or(file_path).to_string()
        }
    }
}