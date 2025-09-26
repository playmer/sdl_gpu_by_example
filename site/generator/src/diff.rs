use std::path::Path;

use similar::{ChangeTag, TextDiff};

pub fn generate_diff_html() {
    let source = "E:/Repos/sdl_gpu_by_example/code/source/003_Triangle_and_Fullscreen_Triangle/003_Triangle_and_Fullscreen_Triangle.c";
    let dest = "E:/Repos/sdl_gpu_by_example/code/source/004_Uniform_Buffers/004_Uniform_Buffers.c";


    let old_content = std::fs::read_to_string(Path::new(source)).unwrap().replace("\r\n","\n");
    let new_content = std::fs::read_to_string(Path::new(dest)).unwrap().replace("\r\n","\n");

    let diff = TextDiff::from_lines(&old_content, &new_content);

    for change in diff.iter_all_changes() {
        let (class, text) = match change.tag() {
            ChangeTag::Delete => ("delete", change.as_str()),
            ChangeTag::Insert => ("insert", change.as_str()),
            ChangeTag::Equal =>  ("      ", change.as_str()),
        };

        if !class.is_empty() {
            print!("--[{}]> {}", class, text.unwrap());
        } else {
            print!("--[{}]> {}", class, text.unwrap());
        }
    }
}


use syntect::easy::HighlightLines;
use syntect::highlighting::Theme;
use syntect::highlighting::ThemeSet;
use syntect::html::append_highlighted_html_for_styled_line;
use syntect::html::highlighted_html_for_string;
use syntect::html::start_highlighted_html_snippet;
use syntect::html::IncludeBackground;
use syntect::parsing::SyntaxReference;
use syntect::parsing::SyntaxSet;

use pulldown_cmark::html;
use pulldown_cmark::Event;
use pulldown_cmark::Options;
use pulldown_cmark::Parser;
use pulldown_cmark::Tag;
use syntect::util::LinesWithEndings;
use syntect::Error;

use std::borrow::Cow;
use std::io;
use std::io::Read;

fn diff(old_content: &Path, new_content: &Path) -> (Vec<ChangeTag>, String, String)
{
    let old_content = std::fs::read_to_string(old_content).unwrap();
    let new_content = std::fs::read_to_string(new_content).unwrap();

    let mut diffed_html = String::new();

    let mut changes : Vec<ChangeTag> = Vec::new();
    let mut full_content = String::with_capacity(old_content.len() + new_content.len());

    let diff = TextDiff::from_lines(&old_content, &new_content);

    for change in diff.iter_all_changes() {
        changes.push(change.tag());

        let line = change.as_str().unwrap();
        full_content.push_str(line);

        match change.tag() {
            ChangeTag::Delete => diffed_html.push_str(&format!("<span class=\"delete\">{}</span>", line)),
            ChangeTag::Insert => diffed_html.push_str(&format!("<span class=\"insert\">{}</span>", line)),
            ChangeTag::Equal => diffed_html.push_str(&format!("{}", line)),
        };
    }

    return (changes, full_content, diffed_html);
}

fn highlighted_html_for_string_local(
    s: &str,
    ss: &SyntaxSet,
    syntax: &SyntaxReference,
    theme: &Theme,
) -> Result<String, Error> {
    let mut highlighter = HighlightLines::new(syntax, theme);
    let (mut output, bg) = start_highlighted_html_snippet(theme);

    //for line in LinesWithEndings::from(s) {
    for line in s.lines() {
        let regions = highlighter.highlight_line(line, ss)?;
        append_highlighted_html_for_styled_line(
            &regions[..],
            IncludeBackground::IfDifferent(bg),
            &mut output,
        )?;
        output.push('\n');
    }
    output.push_str("</pre>\n");
    Ok(output)
}

pub fn diff_and_highlight()
{
    // Setup for pulldown_cmark to read (only) from stdin
    // let opts = Options::empty();
    // let mut input = String::new();
    // io::stdin().read_to_string(&mut input).unwrap();
    // let mut s = String::with_capacity(&input.len() * 3 / 2);
    // let p = Parser::new_ext(&input, opts);
    
    let source = "E:/Repos/sdl_gpu_by_example/code/source/003_Triangle_and_Fullscreen_Triangle/003_Triangle_and_Fullscreen_Triangle.c";
    let dest = "E:/Repos/sdl_gpu_by_example/code/source/004_Uniform_Buffers/004_Uniform_Buffers.c";

    

    // Setup for syntect to highlight (specifically) Rust code
    let default_syntax_set = SyntaxSet::load_defaults_newlines();
    let default_theme_set = ThemeSet::load_defaults();
    let syntax = default_syntax_set.find_syntax_by_extension("c").unwrap();
    let theme = &default_theme_set.themes["base16-ocean.dark"];

    let (changes, full_content, diffed_html) = diff(Path::new(source), Path::new(dest));
    
    //let html = highlighted_html_for_string_local(&full_content, &default_syntax_set, &syntax, &theme).unwrap();
    let html = highlighted_html_for_string(&full_content, &default_syntax_set, &syntax, &theme).unwrap();
    let html = html.replace("\n</span>", "</span>\n");
    let html_lines : Vec<&str> = html.lines().collect();

    let mut highlighted_html = String::with_capacity(html.len() * 2);


    highlighted_html.push_str(
        r#"<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>HTML Diff</title>
    <style>
        .delete { background-color: #ffcccc; text-decoration: line-through; }
        .insert { background-color: #ccffcc; }
    </style>
</head>
<body>
    <h1>Diff for: "#,
    );

    
    highlighted_html.push_str(html_lines[0]);
    for (i, change) in changes.iter().enumerate() {
        match change {
            ChangeTag::Delete => highlighted_html.push_str(&format!("<span class=\"delete\">{}</span>\n", html_lines[i + 1])),
            ChangeTag::Insert => highlighted_html.push_str(&format!("<span class=\"insert\">{}</span>\n", html_lines[i + 1])),
            ChangeTag::Equal => highlighted_html.push_str(&format!("{}\n", html_lines[i + 1])),
        };
    }
    highlighted_html.push_str(html_lines[html_lines.len() - 1]);

    //highlighted_html.push_str("\n\n\nblah\n\n\n");
    //highlighted_html.push_str(&diffed_html);

    highlighted_html.push_str("</pre></body></html>");

    
    print!("{}", highlighted_html);
    
    //highlighted_html.push_str("end\n");
    //println!("{}, {}", changes.len(), html_lines.len());
    //for line in html_lines {
    //    println!("{}", line);
    //}


    // let old_content = std::fs::read_to_string(Path::new(source)).unwrap();
    // let new_content = std::fs::read_to_string(Path::new(dest)).unwrap();

    // let diff = TextDiff::from_lines(&old_content, &new_content);

    // for change in diff.iter_all_changes() {
    //     let (class, text) = match change.tag() {
    //         ChangeTag::Delete => ("delete", change.as_str()),
    //         ChangeTag::Insert => ("insert", change.as_str()),
    //         ChangeTag::Equal =>  ("      ", change.as_str()),
    //     };

    //     if !class.is_empty() {
    //         print!("--[{}]> {}", class, text.unwrap());
    //     } else {
    //         print!("--[{}]> {}", class, text.unwrap());
    //     }
    // }
}
