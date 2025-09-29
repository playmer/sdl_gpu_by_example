
use std::collections::HashMap;
use std::env;
use std::fs::create_dir_all;
use std::fs::File;
use std::io::stdout;
use std::io::{Read, Write};
use std::ops::Index;
use std::path::{Path, PathBuf};
use std::process::Command;
use std::sync::Arc;
use std::sync::RwLock;

use std::thread;
use std::{fmt, fs};
use extract_frontmatter::config::Splitter;
use extract_frontmatter::Extractor;
use handlebars::Handlebars;

use natural_sort_rs::NaturalSort;
use serde_json::Value;
use walkdir::WalkDir;
use yaml_rust::{Yaml, YamlLoader};
use zip::write::SimpleFileOptions;

pub mod diff;

static CODE_DIR: &str =  "../../code";
static CODE_SOURCE_DIR: &str =  "../../code/source";
static CODE_ASSET_DIR: &str =  "../../code/Assets";
static CODE_CMAKE_DIR: &str =  "../../code/CMake";

static CONTENT_DIR: &str =  "../content";
static STATIC_DATA_DIR: &str =  "../static_data";
static TEMPLATE_DIR: &str =  "../template";

static OUTPUT_DIR: &str =  "output";
static NO_ESCAPE: &str =  "<!-- NO_ESCAPE -->";


static COLLAPSIBLE_CARD_START_TEMPLATE: &str = 
"<p class=\"d-inline-flex gap-1\">
  <button class=\"btn btn-primary\" type=\"button\" data-bs-toggle=\"collapse\" data-bs-target=\"#var\" aria-expanded=\"false\" aria-controls=\"var\">
    Expand: CARD_START_TEMPLATE_TEXT
  </button>
</p>
<div class=\"collapse\" id=\"var\">
<div class=\"card card-body\">";

static COLLAPSIBLE_CARD_END_TEMPLATE: &str = 
"</div>
</div>
<br />";

static CARD_START_TEMPLATE: &str = 
"<div class=\"card card-body\">
";

static CARD_END_TEMPLATE: &str = 
"</div>
<br />";


// implement by a structure impls HelperDef
#[derive(Clone)]
struct CollapsibleCardStartHelper
{
    i: Arc<RwLock<u64>>
}

impl handlebars::HelperDef for CollapsibleCardStartHelper {
  fn call<'reg: 'rc, 'rc>(
    &self,
    h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output) -> handlebars::HelperResult
{
    let mut i = self.i.write().unwrap();
    *i += 1;

    let i = i.to_string();
    
    let card_text = h.param(0).unwrap().value().as_str().unwrap();
    let card_start_html = COLLAPSIBLE_CARD_START_TEMPLATE
        .replace("var", &i.to_string())
        .replace("CARD_START_TEMPLATE_TEXT", card_text);

    out.write(&card_start_html).unwrap();
    Ok(())
  }
}
fn collapsible_card_end_helper(
    _h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output) -> handlebars::HelperResult 
{
    out.write(COLLAPSIBLE_CARD_END_TEMPLATE).unwrap();
    Ok(())
}

fn card_start_helper(
    _h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output) -> handlebars::HelperResult 
{
    out.write(CARD_START_TEMPLATE).unwrap();
    Ok(())
}

fn card_end_helper(
    _h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output) -> handlebars::HelperResult 
{
    out.write(CARD_END_TEMPLATE).unwrap();
    Ok(())
}

fn image_helper(
    h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output) -> handlebars::HelperResult 
{
    let description = h.param(0).unwrap();
    let url = h.param(1).unwrap();

    let img_html = format!("<img src={} class=\"img-fluid\" alt=\"{}\">",
        description.value().as_str().unwrap(),
        url.value().as_str().unwrap()
    );

    out.write(&img_html).unwrap();
    Ok(())
}

fn get_folders_or_paths(asset_dir: &Path, want_dirs: bool) -> Vec<PathBuf>
{
    let mut paths : Vec<PathBuf> = Vec::new();

    let walkdir = WalkDir::new(asset_dir);
    let it = &mut walkdir
        .into_iter()
        .filter_map(|e| e.ok());

    for entry in it {
        let entry_path_buf = entry.into_path();
        let entry_path = entry_path_buf.strip_prefix(asset_dir).unwrap();

        if entry_path.as_os_str().is_empty() {
            continue;
        }

        if want_dirs {
            if 1 != entry_path.components().count() {
                continue
            }

            if entry_path_buf.is_dir() {
                paths.push(entry_path.to_path_buf());
            }
        } 
        else if entry_path_buf.is_file() {
            paths.push(entry_path.to_path_buf());
        }
    }

    
    paths.natural_sort_by_key::<str, _, _>(|x| x.to_str().unwrap().to_string() );


    paths
}

fn get_folders(asset_dir: &Path) -> Vec<PathBuf> {
    get_folders_or_paths(asset_dir, true)
}

fn get_files(asset_dir: &Path) -> Vec<PathBuf> {
    get_folders_or_paths(asset_dir, false)
}

struct LessonCode {
    lesson_name: String,
    lesson_code_directory: PathBuf,
    code_files: Vec<PathBuf>,
    code_assets: Vec<PathBuf>
}


impl fmt::Display for LessonCode {
    // This trait requires `fmt` with this exact signature.
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.lesson_name).unwrap();
        write!(f, "\n\t{}", self.lesson_code_directory.display()).unwrap();

        write!(f, "\n\tCode Assets").unwrap();
        for asset in &self.code_assets {
            write!(f, "\n\t\t{}", asset.display()).unwrap();
        }
        
        write!(f, "\n\tCode File").unwrap();
        for asset in &self.code_files {
            write!(f, "\n\t\t{}", asset.display()).unwrap();
        }

        write!(f, "\n\n")
    }
}

fn get_code_assets_lesson_needs(assets: &Vec<PathBuf>, lesson_c_source_path: &Path) -> Vec<PathBuf> {
    
    let mut assets_lesson_needs : Vec<PathBuf> = Vec::new();
    
    let c_source = std::fs::read_to_string(lesson_c_source_path);

    if c_source.is_err() {
        return assets_lesson_needs;
    }

    let c_source = c_source.unwrap();

    for file_asset in assets {
        let file_name = file_asset.file_name().unwrap();

        if c_source.contains(file_name.to_str().unwrap()) {
            assets_lesson_needs.push(file_asset.clone());
        }
    }
    
    assets_lesson_needs
}

fn get_specific_lesson_code() -> Vec<LessonCode> {
    let source_dir = Path::new(CODE_SOURCE_DIR);
    let code_asset_dir = Path::new(CODE_ASSET_DIR);
    let mut lessons: Vec<LessonCode> = Vec::new();

    let assets = {
        let asset_final_dir = Path::new(code_asset_dir.file_name().unwrap());
        
        get_files(code_asset_dir)
            .into_iter()
            .map(|i| asset_final_dir.join(i))
            .collect()
    };

    
    for path in get_folders(source_dir) {
        let lesson_name = path;
        let lesson_code_directory = source_dir.join(&lesson_name);
        let lesson_c_source_path: PathBuf = lesson_code_directory.join(&lesson_name).with_extension("c");

        let code_assets = get_code_assets_lesson_needs(&assets, &lesson_c_source_path);
        let code_files = get_files(&lesson_code_directory);

        lessons.push(LessonCode { 
            lesson_name: lesson_name.to_str().unwrap().to_string(),
            lesson_code_directory: lesson_code_directory.to_path_buf(),
            code_files,
            code_assets
        });
    }

    lessons.natural_sort_by_key::<str, _, _>(|x| x.lesson_name.clone());

    lessons
}

fn get_agnostic_lesson_code() -> Vec<PathBuf> {
    let cmake_dir = Path::new(CODE_CMAKE_DIR);
    let cmake_final_dir = Path::new(cmake_dir.file_name().unwrap());
    
    get_files(cmake_dir)
        .into_iter()
        .map(|i| cmake_final_dir.join(i))
        .collect()
}


fn write_lesson_zips(output_dir: &Path) {
    println!("Writing lesson zips");

    let code_dir = Path::new(CODE_DIR);
    let output_code_dir = output_dir.join("assets").join("code");
    let agnostic_code_for_lessons = get_agnostic_lesson_code();

    fs::create_dir_all(&output_code_dir).unwrap();

    let mut handles = Vec::new();

    for lesson_code in get_specific_lesson_code() {
        // Clones for the thread we're spawning.
        let output_code_dir = output_code_dir.clone();
        let agnostic_code_for_lessons = agnostic_code_for_lessons.clone();

        handles.push(thread::spawn(move || {
            let zip_file_path = output_code_dir.join(lesson_code.lesson_name).with_extension("zip");
            let zip_file = File::create(&zip_file_path).unwrap();
            let mut zip = zip::ZipWriter::new(zip_file);
            let mut buffer = Vec::new();
            
            let options = SimpleFileOptions::default()
                .compression_method(zip::CompressionMethod::Bzip2)
                .unix_permissions(0o755);

            let files = {
                let mut files = Vec::new();
                files.extend_from_slice(&lesson_code.code_assets);
                files.extend_from_slice(&agnostic_code_for_lessons);
                files
            };

            for file in &files {
                zip.start_file_from_path(file, options).unwrap();
                
                let mut f = File::open(code_dir.join(file)).unwrap();
                f.read_to_end(&mut buffer).unwrap();
                zip.write_all(&buffer).unwrap();
                buffer.clear();
            }
            
            for file in &lesson_code.code_files {
                println!("\t{}", file.display());
                zip.start_file_from_path(file, options).unwrap();
                
                let mut f = File::open(lesson_code.lesson_code_directory.join(file)).unwrap();
                f.read_to_end(&mut buffer).unwrap();
                zip.write_all(&buffer).unwrap();
                buffer.clear();
            }

            zip.finish().unwrap();

            println!("Finished zipping {}", zip_file_path.as_path().display());
            stdout().flush().unwrap();
        }));
    }

    for handle in handles {
        handle.join().unwrap();
    }
}

fn write_static_data(output_dir: &Path) {
    let static_data_dir = Path::new(STATIC_DATA_DIR);

    for file_source in get_files(static_data_dir) {
        let file_destination = output_dir.join(&file_source);

        fs::create_dir_all(file_destination.parent().unwrap()).unwrap();
        fs::copy(static_data_dir.join(&file_source), &file_destination).unwrap();
    }
}

struct Content {
    file_name: String,
    file_path: PathBuf,
    front_matter: Yaml,
    markdown: String,
}

fn get_content() -> Vec<Content> {
    let content_dir = Path::new(CONTENT_DIR);

    let mut content: Vec<Content> = Vec::new();

    for file_path in get_files(content_dir) {
        let file_name = file_path.file_name().unwrap().to_str().unwrap().to_string();
        println!("Getting Content for {file_name}");
        stdout().flush().unwrap();

        let front_matter_and_markdown: String = std::fs::read_to_string(content_dir.join(&file_path)).unwrap();
        
        let (front_matter, markdown) = Extractor::new(Splitter::EnclosingLines("---"))
            .extract(&front_matter_and_markdown);

        let docs = YamlLoader::load_from_str(&front_matter).unwrap();

        content.push(Content { 
            file_name,
            file_path, 
            front_matter: docs[0].clone(), 
            markdown: markdown.to_string() 
        });
    }

    content
}


fn get_collections(content: &Vec<Content>) -> Value {
    let mut collections_to_return: HashMap<String, Vec<&Content>> = HashMap::new();

    for content_file in content {
        let collections = content_file.front_matter["collections"].as_vec();

        let collections = if let Some(collections) = collections {
            collections
        } else {
            continue
        };

        for collection in collections {
            let collection_name = collection.as_str().unwrap();
            
            println!("\tcollection_name: {}", collection_name);

            if let Some(inner_collection) = collections_to_return.get_mut(collection_name) {
                inner_collection.push(content_file);
            } else {
                collections_to_return.insert(collection_name.to_string(), Vec::new());
                let inner_collection = collections_to_return.get_mut(collection_name).unwrap();
                inner_collection.push(content_file);
            }
        }
    }

    let mut collection_map: serde_json::Map<String, Value> = serde_json::Map::new();
    for (name, collection) in collections_to_return {
        collection_map.insert(name.clone(), Value::Array(collection.iter().map(|i| get_content_info(i).into()).collect()));
    }

    collection_map.into()
}

fn get_content_info(content: &Content) -> serde_json::Map<String, Value> {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    map.insert("file_name".to_string(), Value::String(content.file_name.clone()));
    map.insert("file_name_no_ext".to_string(), Value::String(content.file_path.file_stem().unwrap().to_str().unwrap().to_string()));
    map.insert("file_path".to_string(), Value::String(content.file_path.to_str().unwrap().to_string()));
    map.insert("title".to_string(), Value::String(content.front_matter["title"].as_str().unwrap().to_string()));
    map.insert("description".to_string(), Value::String(content.front_matter["description"].as_str().unwrap().to_string()));
    map.insert("url".to_string(), Value::String(content.file_path.with_extension("html").to_str().unwrap().to_string().replace("\\", "/")));

    map
}

fn get_content_infos(content: &Vec<Content>) -> Value {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    for content_file in content {
        map.insert(content_file.file_name.clone(), get_content_info(content_file).into());
    }

    Value::Object(map)
}

fn get_template_context(content: &Vec<Content>) -> serde_json::Map<String, Value> {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    map.insert("contents".to_string(), get_content_infos(content));
    map.insert("collections".to_string(), get_collections(content));
    map
}

fn get_specific_content_context(handlebars: &Handlebars<'_>, template_context: &serde_json::Map<String, Value>, inserts: &Vec<(String, String)>, content: &Content) -> serde_json::Map<String, Value> {
    let mut map: serde_json::Map<String, Value> = template_context.clone();
    let mut current_content = get_content_info(content);
        
    let content_title = content.front_matter["title"].as_str().unwrap().to_string();

    let (current_content, inserts) = {
        let mut temp_map = {
            let mut temp_map = map.clone();
            let temp_current_context = current_content.clone();
            temp_map.insert("current_content".to_string(), temp_current_context.into());
            temp_map
        };

        let rendered_content_html = {
            let (rendered_html, toc_items) = diff::parse_markdown_to_html(&content_title, &content.markdown);

            let rendered_html = format!(
                "{}\n{}", 
                NO_ESCAPE, 
                rendered_html
            );
            
            temp_map.insert("table_of_contents".to_string(), toc_items.unwrap());
            rendered_html
        };

        let inserts_value = {
            let context_for_inserts: Value = temp_map.clone().into();
        
            let mut insert_objects: serde_json::Map<String, Value> = serde_json::Map::new();

            for (name, html) in inserts {
                insert_objects
                    .insert(
                        name.clone(), 
                        handlebars.render_template(html, &context_for_inserts)
                    .unwrap().into());
            }
            insert_objects
        };

        temp_map.insert("inserts".to_string(), inserts_value.clone().into());

        // We now have everything we might need to re-render the markdown into HTML
        let context_for_content_render: Value = temp_map.clone().into();
        
        let html = handlebars.render_template(
            &rendered_content_html, 
            &context_for_content_render).unwrap();

        current_content.insert("rendered_html".to_string(), html.into());
        (current_content.into(), inserts_value.into())
    };

    map.insert("inserts".to_string(), inserts);
    map.insert("current_content".to_string(), current_content);

    map
}


pub fn handlebars_escape(data: &str) -> String {
    if data.contains(NO_ESCAPE) {
        return data.to_owned();
    }
    
    handlebars::html_escape(data)
}

fn get_inserts() -> Vec<(String, String)> {
    let inserts_dir = Path::new("../inserts");
    let mut inserts = Vec::new();

    for insert in get_files(inserts_dir) {
        let name = insert.file_stem().unwrap().to_str().unwrap().to_string();
        let insert_html = fs::read_to_string(inserts_dir.join(insert)).unwrap();

        inserts.push((name, insert_html));
    }

    inserts
}

fn process_content() -> Vec<(PathBuf, String)> {
    let output_dir = Path::new(OUTPUT_DIR);
    let code_dir = Path::new(CODE_DIR).join("source");

    let template_dir = Path::new(TEMPLATE_DIR);
    let rendered_html: Vec<(PathBuf, String)> = Vec::new();

    let contents = get_content();
    let template_context = get_template_context(&contents);

    let mut handlebars: Handlebars<'_> = Handlebars::new();
    handlebars.register_escape_fn(handlebars_escape);
    handlebars.set_prevent_indent(true);
    
    handlebars.register_helper("img", Box::new(image_helper));
    handlebars.register_helper("collapsible-card", Box::new(CollapsibleCardStartHelper{i: Arc::new(RwLock::new(0))}));
    handlebars.register_helper("collapsible-card-end", Box::new(collapsible_card_end_helper));
    handlebars.register_helper("card", Box::new(card_start_helper));
    handlebars.register_helper("card-end", Box::new(card_end_helper));

    let inserts = get_inserts();

    for i in 0..contents.len() {
        let content = &contents[i];
        let template = content.front_matter["template"].as_str().unwrap();
        
        let previous_content = if (i > 0) && (template == "lesson_template.html") {
            Some(&contents[i - 1])
        } else {
            None
        };

        println!("Processing {} content", content.file_name);
        stdout().flush().unwrap();

        let template_path = template_dir.join(template);
        let template_html = std::fs::read_to_string(&template_path).unwrap();
        let mut current_content_context = get_specific_content_context(&handlebars, &template_context, &inserts, &content);

        // TODO: Probably should figure out a way to not hardcode this.
        if let Some(previous_content) = previous_content {
            let previous_template = previous_content.front_matter["template"].as_str().unwrap();
            if previous_template == "lesson_template.html" {
                let content_name = Path::new(&content.file_name).file_stem().unwrap();
                let previous_content_name = Path::new(&previous_content.file_name).file_stem().unwrap();

                let html = diff::diff_and_highlight(
                    &code_dir.join(previous_content_name).join(format!("{}.c", previous_content_name.display())),
                    &code_dir.join(content_name).join(format!("{}.c", content_name.display()))
                );

                current_content_context.insert("lesson_diff".to_string(), html.into());
            }
        }

        let current_content_context: Value = current_content_context.into();

        let final_html = handlebars.render_template(&template_html, &current_content_context).unwrap();

        let final_file_path = output_dir.join(&content.file_path).with_extension("html");
        create_dir_all(final_file_path.parent().unwrap()).unwrap();

        std::fs::write(final_file_path, final_html).unwrap();
    }

    rendered_html
}


fn main() {
    //diff::diff_and_highlight();

    let output_dir = Path::new(OUTPUT_DIR);

    // Delete existing output    
    if fs::exists(output_dir).unwrap()
    {
        fs::remove_dir_all(output_dir).unwrap();
    }

    let lesson_zip_task = thread::spawn(move || {
        write_lesson_zips(output_dir);
    });

    let static_data_task = thread::spawn(move || {
        write_static_data(output_dir);
    });
    
    static_data_task.join().unwrap();
    lesson_zip_task.join().unwrap();

    process_content();

    let args: Vec<String> = env::args().collect();

    if !args.contains(&"--no-serve".to_owned())
    {
        let gh_output_dir = Path::new("output_github");
        let destination = gh_output_dir.join("sdl_gpu_by_example");

        // Delete existing output    
        if fs::exists(gh_output_dir).unwrap()
        {
            fs::remove_dir_all(gh_output_dir).unwrap();
        }
        
        fs::create_dir_all(&destination).unwrap();

        for file_source in get_files(output_dir) {
            let file_destination = destination.join(&file_source);
            fs::create_dir_all(file_destination.parent().unwrap()).unwrap();
            fs::copy(output_dir.join(&file_source), &file_destination).unwrap();
        }

        println!("Link to site: http://127.0.0.1:4040/sdl_gpu_by_example/");

        // Should run the bottom command to host the site.
        let _ = Command::new("http-serve-folder")
            .args([gh_output_dir])
            .status()
            .expect("failed to execute process");
    }
}
