use std::path::Path;

use pulldown_cmark::CodeBlockKind;
use pulldown_cmark::CowStr;
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
        let (_, _) = match change.tag() {
            ChangeTag::Delete => ("delete", change.as_str()),
            ChangeTag::Insert => ("insert", change.as_str()),
            ChangeTag::Equal =>  ("      ", change.as_str()),
        };
    }
}


use syntect::highlighting::Theme;
use syntect::highlighting::ThemeSet;
use syntect::html::highlighted_html_for_string;
use syntect::parsing::SyntaxSet;

use pulldown_cmark::Event;
use pulldown_cmark::Options;
use pulldown_cmark::Parser;
use pulldown_cmark::Tag;


fn diff(old_content: &Path, new_content: &Path) -> (Vec<ChangeTag>, String)
{
    println!("wooooooooo {}, {}", &old_content.display(), &new_content.display());
    let old_content = std::fs::read_to_string(old_content).unwrap();
    let new_content = std::fs::read_to_string(new_content).unwrap();

    let mut changes : Vec<ChangeTag> = Vec::new();
    let mut full_content = String::with_capacity(old_content.len() + new_content.len());

    let diff = TextDiff::from_lines(&old_content, &new_content);

    for change in diff.iter_all_changes() {
        changes.push(change.tag());

        let line = change.as_str().unwrap();
        full_content.push_str(line);
    }

    (changes, full_content)
}

pub fn diff_and_highlight(source: &Path, dest: &Path) -> String {
    if !std::fs::exists(source).unwrap() || !std::fs::exists(dest).unwrap() {
        return String::new();
    }

    // Setup for syntect to highlight (specifically) Rust code
    let default_syntax_set = SyntaxSet::load_defaults_newlines();
    let default_theme_set = ThemeSet::load_defaults();
    let syntax = default_syntax_set.find_syntax_by_extension("c").unwrap();
    let theme = &default_theme_set.themes["InspiredGitHub"];

    let (changes, full_content) = diff(source, dest);
    
    let html = highlighted_html_for_string(&full_content, &default_syntax_set, syntax, theme).unwrap();
    let html = html.replace("\n</span>", "</span>\n");
    let html_lines : Vec<&str> = html.lines().collect();

    let mut highlighted_html = String::with_capacity(html.len() * 2);


    highlighted_html.push_str("<!-- NO_ESCAPE -->\n");

    
    highlighted_html.push_str(html_lines[0]);
    for (i, change) in changes.iter().enumerate() {
        match change {
            ChangeTag::Delete => highlighted_html.push_str(&format!("<span class=\"delete\">{}</span>\n", html_lines[i + 1])),
            ChangeTag::Insert => highlighted_html.push_str(&format!("<span class=\"insert\">{}</span>\n", html_lines[i + 1])),
            ChangeTag::Equal => highlighted_html.push_str(&format!("{}\n", html_lines[i + 1])),
        };
    }
    highlighted_html.push_str(html_lines[html_lines.len() - 1]);
    
    return highlighted_html;
}

struct Highlighter {
    theme: Theme,
    syntax_set: SyntaxSet,
    to_highlight: String
}

impl Highlighter {
    fn new() -> Highlighter {
        Highlighter{
            theme: ThemeSet::load_defaults().themes["InspiredGitHub"].clone(),
            syntax_set: SyntaxSet::load_defaults_newlines(),
            to_highlight: String::new()
        }
    }
    
    fn highlight_code_block<'a>(&mut self, transformed_events: &mut Vec<Event<'a>>, parser: &mut Parser<'a>, kind: CodeBlockKind<'_>)
    {
        transformed_events.push(Event::Html(CowStr::Boxed("<div class=\"card card-body\">".to_string().into_boxed_str())));

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
        
        for event in parser.by_ref() {
            match event {
                Event::Text(text) => {
                    self.to_highlight.push_str(&text);
                }
                Event::End(TagEnd::CodeBlock) => {
                    let syntax = language_syntax.unwrap();
                    let mut html = highlighted_html_for_string(&self.to_highlight, &self.syntax_set, syntax, &self.theme).unwrap();
                    html.push_str("</div>");
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

    map.into()
}


struct TocGenerator {
    children_stack: Vec<TocItem>,
    last_level: i8,
}

impl TocGenerator {
    fn new(title: String) -> TocGenerator {
        TocGenerator {
            children_stack: vec![ TocItem { 
                level: 1, 
                text: title,
                url: "#title".to_string(), 
                children: Vec::new()
            }],
            last_level: 1
        }
    }
    
    fn string_to_id(value: &str) -> String {
        value
            .replace([' ', '/', '\\', '.', ','], "_")
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
                match &maybe_text_event {
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
                 if item.text != "$" {
                    panic!("Processing a header with name {} and id {}, this header has a heading of 1, which is disallowed in content. Anything above 1 is allowed. Headers must start at 2, and only increase one at a time.",
                        item.text, 
                        item.url);
                 }
                 
                 return;
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

fn process_math<'a>(transformed_events: &mut Vec<Event<'a>>, parser: &mut Parser<'a>, math: CowStr<'_>) {
    let storage = pulldown_latex::Storage::new();
    let parser = pulldown_latex::Parser::new(&math, &storage);
    let mut mathml = String::new();
    let config = Default::default();

    match pulldown_latex::push_mathml(&mut mathml, parser, config) {
        Ok(()) => transformed_events.push(Event::Html(CowStr::Boxed(mathml.into_boxed_str()))),
        Err(e) => eprintln!("Error while rendering: {}", e),
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
    let mut toc_generator = TocGenerator::new(title.to_string());

    let mut parser: Parser<'_> = Parser::new_ext(content, options);
    let mut transformed_events: Vec<Event<'_>> = Vec::new();

    while let Some(event) = parser.next() {
        match event.clone() {
            Event::Start(Tag::Heading { level: _, id: _, classes: _, attrs: _ }) => {
                toc_generator.process_header(&mut transformed_events, &mut parser, event);
            }
            Event::Start(Tag::CodeBlock(kind)) => {
                highlighter.highlight_code_block(&mut transformed_events, &mut parser, kind);
            }
            Event::DisplayMath(math) => {
                process_math(&mut transformed_events, &mut parser, math);
            }
            Event::InlineMath(math) => {
                process_math(&mut transformed_events, &mut parser, math);
            }
            event => {
                transformed_events.push(event);
            }
        }
    }

    // Now we send this new vector of events off to be transformed into HTML
    pulldown_cmark::html::push_html(&mut html_output, transformed_events.into_iter());

    (html_output, toc_generator.get_toc_value())
}
