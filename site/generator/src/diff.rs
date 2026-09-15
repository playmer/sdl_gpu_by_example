use std::path::Path;

use similar::{ChangeTag, TextDiff};

use syntect::highlighting::ThemeSet;
use syntect::html::highlighted_html_for_string;
use syntect::parsing::SyntaxSet;

fn diff(old_content: &Path, new_content: &Path) -> anyhow::Result<(Vec<ChangeTag>, String)> {
    println!(
        "wooooooooo {}, {}",
        &old_content.display(),
        &new_content.display()
    );
    let old_content = std::fs::read_to_string(old_content)?;
    let new_content = std::fs::read_to_string(new_content)?;

    let mut changes: Vec<ChangeTag> = Vec::new();
    let mut full_content = String::with_capacity(old_content.len() + new_content.len());

    let diff = TextDiff::from_lines(&old_content, &new_content);

    for change in diff.iter_all_changes() {
        changes.push(change.tag());

        let line = change.as_str().unwrap();
        full_content.push_str(line);
    }

    Ok((changes, full_content))
}

pub fn diff_and_highlight(source: &Path, dest: &Path) -> anyhow::Result<String> {
    if !std::fs::exists(source).unwrap() || !std::fs::exists(dest).unwrap() {
        return Ok(String::new());
    }

    // Setup for syntect to highlight (specifically) Rust code
    let default_syntax_set = SyntaxSet::load_defaults_newlines();
    let default_theme_set = ThemeSet::load_defaults();
    let syntax = default_syntax_set.find_syntax_by_extension("c").unwrap();
    let theme = &default_theme_set.themes["InspiredGitHub"];

    let (changes, full_content) = diff(source, dest)?;

    let html =
        highlighted_html_for_string(&full_content, &default_syntax_set, syntax, theme).unwrap();
    let html = html.replace("\n</span>", "</span>\n");
    let html_lines: Vec<&str> = html.lines().collect();

    let mut highlighted_html = String::with_capacity(html.len() * 2);

    highlighted_html.push_str("<!-- NO_ESCAPE -->\n");

    highlighted_html.push_str(html_lines[0]);
    for (i, change) in changes.iter().enumerate() {
        match change {
            ChangeTag::Delete => highlighted_html.push_str(&format!(
                "<span class=\"delete\">{}</span>\n",
                html_lines[i + 1]
            )),
            ChangeTag::Insert => highlighted_html.push_str(&format!(
                "<span class=\"insert\">{}</span>\n",
                html_lines[i + 1]
            )),
            ChangeTag::Equal => highlighted_html.push_str(&format!("{}\n", html_lines[i + 1])),
        };
    }
    highlighted_html.push_str(html_lines[html_lines.len() - 1]);

    return Ok(highlighted_html);
}
