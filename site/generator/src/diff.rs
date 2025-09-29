use std::path::Path;

use pulldown_cmark::CodeBlockKind;
use pulldown_cmark::CowStr;
use pulldown_cmark::HeadingLevel;
use pulldown_cmark::TagEnd;
use similar::{ChangeTag, TextDiff};
use serde_json::Value;

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
use syntect::parsing::Regex;
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

struct Highlighter {
    theme: Theme,
    syntax_set: SyntaxSet,
    to_highlight: String
}

impl Highlighter {
    fn new() -> Highlighter {
        return Highlighter{
            theme: ThemeSet::load_defaults().themes["InspiredGitHub"].clone(),
            syntax_set: SyntaxSet::load_defaults_newlines(),
            to_highlight: String::new()
        };
    }
    
    fn highlight_code_block<'a>(&mut self, transformed_events: &mut Vec<Event<'a>>, parser: &mut Parser<'a>, kind: CodeBlockKind<'_>)
    {
        let language_syntax = match kind {
            CodeBlockKind::Fenced(lang) => {
                if let Some(found_syntax) = self.syntax_set.find_syntax_by_extension(&lang) {
                    Some(found_syntax)
                } else {
                    Some(self.syntax_set.find_syntax_plain_text())
                }
            },
            CodeBlockKind::Indented => {
                Some(self.syntax_set.find_syntax_plain_text())
            }
        };
        
        while let Some(event) = parser.next() {
            match event {
                Event::Text(text) => {
                    self.to_highlight.push_str(&text);
                }
                Event::End(TagEnd::CodeBlock) => {
                    let syntax = language_syntax.unwrap();
                    let html = highlighted_html_for_string(&self.to_highlight, &self.syntax_set, &syntax, &self.theme).unwrap();

                    transformed_events.push(Event::Html(CowStr::Boxed(html.into_boxed_str())));
                    self.to_highlight.clear();
                    return;
                }    
                e => {
                    transformed_events.push(e);
                }
            }   
        }
    }
}

pub struct TocItem {
    pub level: i8,
    pub text: String,
    pub url: String,
    pub children: Vec<TocItem>
}

pub fn process_root_toc(item: &TocItem) -> Value {
    let mut children: Vec<Value> = Vec::new();

    for child in &item.children {
        children.push(process_root_toc(child));
    }

    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();
    map.insert("name".to_string(), item.text.clone().into());
    map.insert("url".to_string(),  item.url.clone().into());
    map.insert("children".to_string(),  children.into());

    return map.into();
}


struct TocGenerator {
    children_stack: Vec<TocItem>,
    last_level: i8,
}

impl TocGenerator {
    fn new(title: String) -> TocGenerator {
        let mut children_stack: Vec<TocItem> = Vec::new();
        children_stack.push(TocItem { 
            level: 1, 
            text: title,
            url: "#title".to_string(), 
            children: Vec::new()
        });

        return TocGenerator {
            children_stack,
            last_level: 1
        }
    }
    
    fn string_to_id(value: &str) -> String {
        return value
            .replace(' ', "_")
            .replace('/', "_")
            .replace('\\', "_")
            .replace('.', "_")
            .replace(',', "_")
            .trim_matches('_')
            .to_ascii_lowercase()
            .replace(|c: char| !c.is_alphanumeric() && (c != '_'), "")
    }

    fn process_header<'a>(&mut self, transformed_events: &mut Vec<Event<'a>>, parser: &mut Parser<'a>, event: Event<'a>) {
        if let Event::Start(Tag::Heading { level, id: _, classes: _, attrs: _ }) = &event {
            print!("\tLevel{{{}}}: ", &level);
            let mut header_text = String::new();
            let header_id;

            if let Some(maybe_text_event) = parser.next() {
                match (&maybe_text_event) {
                    Event::Text(text) => {
                        header_id = TocGenerator::string_to_id(text);
                        header_text = text.to_string();
                    },
                    _ => { 
                        header_id = format!("generated_toc_entry_{}", self.children_stack.len());
                    }
                }

                transformed_events.push(Event::Start(Tag::Heading { 
                    level: *level, 
                    id: Some(CowStr::Boxed(header_id.clone().into_boxed_str())), 
                    classes: Vec::new(), 
                    attrs: Vec::new()
                }));
                
                transformed_events.push(maybe_text_event);
            } else {
                header_id = format!("generated_toc_entry_{}", self.children_stack.len());
                transformed_events.push(event.clone());
            }

            let current_level = level
                .to_string()
                .split_off(1)
                .parse::<i8>()
                .unwrap();

            
            let url = format!("#{}", header_id);

            let item = TocItem{
                level: current_level,
                text: header_text,
                url, 
                children: Vec::new()
            };

            if item.level == 1 {
                panic!("Processing a header with name {} and id {}, this header has a heading of 1, which is disallowed in content. Anything above 1 is allowed. Headers must start at 2, and only increase one at a time.",
                    item.text, 
                    item.url);
            }

            println!("\t{}, {}", self.last_level, item.level);

            if self.last_level < item.level {
                self.last_level += 1;
                self.children_stack.push(item);
            } else if self.last_level == item.level {
                let last_item  = self.children_stack.pop().unwrap();
                self.children_stack.iter_mut().nth_back(0).unwrap().children.push(last_item);
                self.children_stack.push(item);
            }  else if self.last_level >= item.level {
                for _ in 0..(self.last_level - item.level) + 1 {
                    let last_item  = self.children_stack.pop().unwrap();
                    self.children_stack.iter_mut().nth_back(0).unwrap().children.push(last_item);
                    self.last_level -= 1;
                }

                self.children_stack.push(item);
                self.last_level += 1;
            }
        }
    }

    fn get_toc_value(&mut self) -> Option<Value> {
        for _ in 0..(self.children_stack.len() - 1) {
            let last_item  = self.children_stack.pop().unwrap();
            println!("\tget_toc_value: {}", last_item.url);
            self.children_stack.iter_mut().nth_back(0).unwrap().children.push(last_item);
        }

        if self.children_stack.len() != 0 {
            let toc = self.children_stack.first().unwrap();
            //println!("StartToc");
            //print_root_toc(&toc);
            //println!("EndToc");
            return Some(process_root_toc(&toc));
        }

        return None;
    }
}



pub fn parse_markdown_to_html(title: &str, content: &str) -> (String, Option<Value>) {
    // Set up options and parser. Strikethroughs are not part of the CommonMark standard
    // and we therefore must enable it explicitly.
    let mut options = Options::empty();
    options.insert(Options::ENABLE_FOOTNOTES);
    options.insert(Options::ENABLE_STRIKETHROUGH);
    options.insert(Options::ENABLE_MATH);
    options.insert(Options::ENABLE_GFM);
    options.insert(Options::ENABLE_HEADING_ATTRIBUTES);

    let mut html_output = String::new();
    let mut highlighter = Highlighter::new();
    let mut tocGenerator = TocGenerator::new(title.to_string());

    let mut parser: Parser<'_> = Parser::new_ext(content, options);
    let mut transformed_events: Vec<Event<'_>> = Vec::new();

    while let Some(event) = parser.next() {
        match event.clone() {
            Event::Start(Tag::Heading { level: _, id: _, classes: _, attrs: _ }) => {
                tocGenerator.process_header(&mut transformed_events, &mut parser, event);
            }
            Event::Start(Tag::CodeBlock(kind)) => {
                highlighter.highlight_code_block(&mut transformed_events, &mut parser, kind);
            }
            event => {
                transformed_events.push(event);
            }
        }
    }

    // Now we send this new vector of events off to be transformed into HTML
    pulldown_cmark::html::push_html(&mut html_output, transformed_events.into_iter());

    return (html_output, tocGenerator.get_toc_value());
}
