use std::collections::HashMap;
use std::io::Write;
use std::process::Command;
use std::process::Stdio;

use anyhow::Context;
use pulldown_cmark::CodeBlockKind;
use pulldown_cmark::CowStr;
use pulldown_cmark::TagEnd;
use serde_json::Value;

use syntect::highlighting::Theme;
use syntect::highlighting::ThemeSet;
use syntect::html::highlighted_html_for_string;
use syntect::parsing::SyntaxSet;

use pulldown_cmark::Event;
use pulldown_cmark::Options;
use pulldown_cmark::Parser;
use pulldown_cmark::Tag;

struct d2RenderOptions {
    name: String,
    caption: String,
}

fn parse_d2_options(args: Vec<&str>) -> anyhow::Result<d2RenderOptions> {
    let mut arg_map = HashMap::new();

    for arg in args {
        let (key, value) = arg
            .split_once('=')
            .with_context(|| format!("Expected `key=value`, found `{arg:?}`"))?;

        arg_map.insert(key, value);
    }

    Ok(d2RenderOptions {
        name: arg_map
            .remove("name")
            .with_context(|| format!("No name arg found for d2 declaration"))?
            .to_owned(),
        caption: arg_map
            .remove("caption")
            .with_context(|| format!("No caption arg found for d2 declaration"))?
            .to_owned(),
    })
}

fn render_d2(args: Vec<&str>, d2_markup: &str) -> anyhow::Result<String> {
    let options = parse_d2_options(args)?;


    let mut d2 = Command::new("d2")
        .args(["--no-xml-tag", "-", "-"])
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()
        .context("Failed to launch d2.")?;

    d2.stdin
        .take()
        .context("Failed to open d2's stdin")?
        .write_all(d2_markup.as_bytes())?;

    let output = d2.wait_with_output()?;

    if !output.status.success() {
        anyhow::bail!("d2 failed:\n\t{}", String::from_utf8_lossy(&output.stderr));
    }

    Ok(String::from_utf8(output.stdout)?)
}

struct Highlighter {
    theme: Theme,
    syntax_set: SyntaxSet,
}

impl Highlighter {
    fn new() -> Highlighter {
        Highlighter {
            theme: ThemeSet::load_defaults().themes["InspiredGitHub"].clone(),
            syntax_set: SyntaxSet::load_defaults_newlines(),
        }
    }

    fn highlight_code_block<'a>(
        &self,
        transformed_events: &mut Vec<Event<'a>>,
        kind: CodeBlockKind<'_>,
        source: &str,
    ) -> anyhow::Result<()> {
        transformed_events.push(Event::Html(CowStr::Boxed(
            "<div class=\"card card-body\">"
                .to_string()
                .into_boxed_str(),
        )));

        let language_syntax = match kind {
            CodeBlockKind::Fenced(lang) => {
                if let Some(found_syntax) = self.syntax_set.find_syntax_by_extension(&lang) {
                    found_syntax
                } else {
                    self.syntax_set.find_syntax_plain_text()
                }
            }
            CodeBlockKind::Indented => self.syntax_set.find_syntax_plain_text(),
        };

        let mut html =
            highlighted_html_for_string(source, &self.syntax_set, language_syntax, &self.theme)?;
        html.push_str("</div>");
        transformed_events.push(Event::Html(CowStr::Boxed(html.into_boxed_str())));

        Ok(())
    }
}

struct CodeBlockProcessor {
    highlighter: Highlighter,
}

impl CodeBlockProcessor {
    fn new() -> CodeBlockProcessor {
        CodeBlockProcessor {
            highlighter: Highlighter::new(),
        }
    }

    pub fn process_code_block<'a>(
        &mut self,
        transformed_events: &mut Vec<Event<'a>>,
        parser: &mut Parser<'a>,
        kind: CodeBlockKind<'_>,
    ) -> anyhow::Result<()> {
        let mut text = String::new();

        for event in parser.by_ref() {
            match event {
                Event::Text(new_text) => text.push_str(&new_text),
                Event::End(TagEnd::CodeBlock) => break,
                _ => {}
            }
        }

        match kind {
            CodeBlockKind::Fenced(info)
                if let mut args = info.to_owned().split_whitespace()
                    && args.next() == Some("d2") =>
            {
                let args: Vec<&str> = args.collect();
                let svg = render_d2(args, &text)?;
                transformed_events.push(Event::Html(CowStr::Boxed(svg.into_boxed_str())));
            }
            _ => self
                .highlighter
                .highlight_code_block(transformed_events, kind, &text)?,
        }

        Ok(())
    }
}

pub struct TocItem {
    pub level: i8,
    pub text: String,
    pub url: String,
    pub children: Vec<TocItem>,
}

pub fn process_root_toc(item: &TocItem) -> Value {
    let mut children: Vec<Value> = Vec::new();

    for child in &item.children {
        children.push(process_root_toc(child));
    }

    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();
    map.insert("name".to_string(), item.text.clone().into());
    map.insert("url".to_string(), item.url.clone().into());
    map.insert("children".to_string(), children.into());

    map.into()
}

struct TocGenerator {
    children_stack: Vec<TocItem>,
    last_level: i8,
}

impl TocGenerator {
    fn new(title: String) -> TocGenerator {
        TocGenerator {
            children_stack: vec![TocItem {
                level: 1,
                text: title,
                url: "#title".to_string(),
                children: Vec::new(),
            }],
            last_level: 1,
        }
    }

    fn string_to_id(value: &str) -> String {
        value
            .replace([' ', '/', '\\', '.', ','], "_")
            .trim_matches('_')
            .to_ascii_lowercase()
            .replace(|c: char| !c.is_alphanumeric() && (c != '_'), "")
    }

    fn process_header<'a>(
        &mut self,
        transformed_events: &mut Vec<Event<'a>>,
        parser: &mut Parser<'a>,
        event: Event<'a>,
    ) {
        if let Event::Start(Tag::Heading {
            level,
            id: _,
            classes: _,
            attrs: _,
        }) = &event
        {
            print!("\tLevel{{{}}}: ", &level);
            let mut header_text = String::new();
            let header_id;

            if let Some(maybe_text_event) = parser.next() {
                match &maybe_text_event {
                    Event::Text(text) => {
                        header_id = TocGenerator::string_to_id(text);
                        header_text = text.to_string();
                    }
                    _ => {
                        header_id = format!("generated_toc_entry_{}", self.children_stack.len());
                    }
                }

                transformed_events.push(Event::Start(Tag::Heading {
                    level: *level,
                    id: Some(CowStr::Boxed(header_id.clone().into_boxed_str())),
                    classes: Vec::new(),
                    attrs: Vec::new(),
                }));

                transformed_events.push(maybe_text_event);
            } else {
                header_id = format!("generated_toc_entry_{}", self.children_stack.len());
                transformed_events.push(event.clone());
            }

            let current_level = level.to_string().split_off(1).parse::<i8>().unwrap();

            let url = format!("#{}", header_id);

            let item = TocItem {
                level: current_level,
                text: header_text,
                url,
                children: Vec::new(),
            };

            if item.level == 1 {
                if item.text != "$" {
                    panic!(
                        "Processing a header with name {} and id {}, this header has a heading of 1, which is disallowed in content. Anything above 1 is allowed. Headers must start at 2, and only increase one at a time.",
                        item.text, item.url
                    );
                }

                return;
            }

            println!("\t{}, {}", self.last_level, item.level);

            if self.last_level < item.level {
                self.last_level += 1;
                self.children_stack.push(item);
            } else if self.last_level == item.level {
                let last_item = self.children_stack.pop().unwrap();
                self.children_stack
                    .iter_mut()
                    .nth_back(0)
                    .unwrap()
                    .children
                    .push(last_item);
                self.children_stack.push(item);
            } else if self.last_level >= item.level {
                for _ in 0..(self.last_level - item.level) + 1 {
                    let last_item = self.children_stack.pop().unwrap();
                    self.children_stack
                        .iter_mut()
                        .nth_back(0)
                        .unwrap()
                        .children
                        .push(last_item);
                    self.last_level -= 1;
                }

                self.children_stack.push(item);
                self.last_level += 1;
            }
        }
    }

    fn get_toc_value(&mut self) -> Option<Value> {
        for _ in 0..(self.children_stack.len() - 1) {
            let last_item = self.children_stack.pop().unwrap();
            println!("\tget_toc_value: {}", last_item.url);
            self.children_stack
                .iter_mut()
                .nth_back(0)
                .unwrap()
                .children
                .push(last_item);
        }

        if !self.children_stack.is_empty() {
            let toc = self.children_stack.first().unwrap();
            //println!("StartToc");
            //print_root_toc(&toc);
            //println!("EndToc");
            return Some(process_root_toc(toc));
        }

        None
    }
}

fn process_math<'a>(transformed_events: &mut Vec<Event<'a>>, math: CowStr<'_>) {
    let storage = pulldown_latex::Storage::new();
    let parser = pulldown_latex::Parser::new(&math, &storage);
    let mut mathml = String::new();
    let config = Default::default();

    match pulldown_latex::push_mathml(&mut mathml, parser, config) {
        Ok(()) => transformed_events.push(Event::Html(CowStr::Boxed(mathml.into_boxed_str()))),
        Err(e) => eprintln!("Error while rendering: {}", e),
    }
}

pub fn parse_markdown_to_html(
    title: &str,
    content: &str,
) -> anyhow::Result<(String, Option<Value>)> {
    // Set up options and parser. Strikethroughs are not part of the CommonMark standard
    // and we therefore must enable it explicitly.
    let mut options = Options::empty();
    options.insert(Options::ENABLE_FOOTNOTES);
    options.insert(Options::ENABLE_STRIKETHROUGH);
    options.insert(Options::ENABLE_MATH);
    options.insert(Options::ENABLE_GFM);
    options.insert(Options::ENABLE_HEADING_ATTRIBUTES);

    let mut html_output = String::new();
    let mut code_block_processor = CodeBlockProcessor::new();
    let mut toc_generator = TocGenerator::new(title.to_string());

    let mut parser: Parser<'_> = Parser::new_ext(content, options);
    let mut transformed_events: Vec<Event<'_>> = Vec::new();

    while let Some(event) = parser.next() {
        match event.clone() {
            Event::Start(Tag::Heading {
                level: _,
                id: _,
                classes: _,
                attrs: _,
            }) => {
                toc_generator.process_header(&mut transformed_events, &mut parser, event);
            }
            Event::Start(Tag::CodeBlock(kind)) => {
                code_block_processor.process_code_block(
                    &mut transformed_events,
                    &mut parser,
                    kind,
                )?;
            }
            Event::DisplayMath(math) => {
                process_math(&mut transformed_events, math);
            }
            Event::InlineMath(math) => {
                process_math(&mut transformed_events, math);
            }
            event => {
                transformed_events.push(event);
            }
        }
    }

    // Now we send this new vector of events off to be transformed into HTML
    pulldown_cmark::html::push_html(&mut html_output, transformed_events.into_iter());

    Ok((html_output, toc_generator.get_toc_value()))
}
