
use std::collections::HashMap;
use std::fs::create_dir_all;
use std::fs::File;
use std::io::{Read, Write};
use std::path::{Path, PathBuf};
use std::process::Command;
use std::thread::panicking;
//use std::str::pattern::Pattern;
use std::{env, fmt, fs};
use extract_frontmatter::config::Splitter;
use extract_frontmatter::Extractor;
use handlebars::Handlebars;

use markdown::CompileOptions;
use markdown::Options;
use markdown;
use natural_sort_rs::NaturalSort;
use scraper::ElementRef;
use scraper::{Html, Selector};
use serde_json::Value;
use walkdir::WalkDir;
use yaml_rust::{Yaml, YamlLoader};
use zip::write::SimpleFileOptions;

static CODE_DIR: &str =  "../../code";
static CODE_SOURCE_DIR: &str =  "../../code/source";
static CODE_ASSET_DIR: &str =  "../../code/Assets";
static CODE_CMAKE_DIR: &str =  "../../code/CMake";

static CONTENT_DIR: &str =  "../content";
static STATIC_DATA_DIR: &str =  "../static_data";
static TEMPLATE_DIR: &str =  "../template";

static OUTPUT_DIR: &str =  "output";
static NO_ESCAPE: &str =  "<!-- NO_ESCAPE -->\n";




//     let post_content = markdown::to_html_with_options(&markdown_data, &front_matter).unwrap();

// struct Post {
//     title: String,
//     description: String, 
//     url: String,
//     content: String
// }

// fn parse_post(file_name: &str, markdown_data: &String, post_template: &str) -> Post {
//     let front_matter = Options {
//         parse: ParseOptions {
//             constructs: Constructs {
//                 frontmatter: true,
//                 ..Default::default()
//             },
//             ..Default::default()
//         },
//         ..Default::default()
//     };

//     let ast = to_mdast(markdown_data, &front_matter.parse).unwrap();

//     let mut title = String::new();
//     let mut description = String::new();

//     let yaml = ast.children().unwrap()[0].to_string();
//     let yaml : Vec<&str> = yaml.split('\n').collect();
//     for entry in yaml {
//         let entry = entry.trim();
//         if entry.starts_with("title:") {
//             let mut splits : Vec<&str> = entry.split("title:").collect();
//             splits.retain(|&x| !x.trim().is_empty());
            
//             //println!("hit title: [{}]", splits[0]);
//             title = splits[0].trim().to_owned();
//         }
//         else if entry.starts_with("description:") {
//             let mut splits : Vec<&str> = entry.split("description:").collect();
//             splits.retain(|&x| !x.trim().is_empty());
//             //println!("hit description: {}", splits[0]);
//             description = splits[0].trim().to_owned();
//         }
//     }

//     if title.len() == 0 {
//         panic!("Couldn't find a title for {file_name}");
//     }
    
//     if description.len() == 0 {
//         panic!("Couldn't find a description for {file_name}");
//     }
    
//     let post_content = markdown::to_html_with_options(&markdown_data, &front_matter).unwrap();

//     return Post{
//         title: title.clone(),
//         description,
//         url: file_name.replace("md", "html"),
//         content: post_template
//         .replace("<<<<<<<<POST_TITLE>>>>>>>>", &title)
//         .replace("<<<<<<<<POST_CONTENT>>>>>>>>", &post_content)
//     };
// }


// // Need to figure out a way to dump images into the html like this
// const IMAGE_TEMPLATE: &str = "
// <p>
//   <a href=\"assets/images/002_Window_and_Clearing__Running.jpg\" target=\"_blank\">
//     <img src=\"assets/images/002_Window_and_Clearing__Running.jpg\" style='width:100%;' border=\"0\" alt=\"Null\">
//   </a>
// </p>
// ";

// const INDEX_ENTRY : &str = "

// <h4 class=\"<<<<<<<<POST_TITLE>>>>>>>>\">
// <a href=\"<<<<<<<<POST_URL>>>>>>>>\"><<<<<<<<POST_TITLE>>>>>>>></a>
// </h4>
// <p><<<<<<<<POST_DESCRIPTION>>>>>>>></p>

// <hr>

// ";

// fn get_index_entries(posts: &Vec<Post>) -> String
// {
//     let mut index_content = String::new();

//     for (i, post) in posts.iter().enumerate()
//     {
//         index_content += &INDEX_ENTRY
//             .replace("<<<<<<<<POST_TITLE>>>>>>>>", &format!("{}. {}", i + 1, post.title))
//             .replace("<<<<<<<<POST_URL>>>>>>>>", &post.url)
//             .replace("<<<<<<<<POST_DESCRIPTION>>>>>>>>", &post.description);
//     }

//     return index_content;
// }

// fn get_lesson_paths() -> Vec<(Option<PathBuf>, PathBuf, Option<PathBuf>)> {
//     let mut final_paths: Vec<(Option<PathBuf>, PathBuf, Option<PathBuf>)> = Vec::new();

//     let paths = {
//         let mut paths: Vec<PathBuf> = Vec::new();

//         for entry in fs::read_dir("../content/lessons").unwrap() {
//             paths.push(entry.unwrap().path().to_path_buf());
//         }
//         paths
//     };

//     for i in 0..paths.len() 
//     {
//         let last = if i == 0 {
//             None
//         } else {
//             Some(paths[i - 1].clone())
//         };

//         let next = if (i + 1) < paths.len() {
//             Some(paths[i + 1].clone())
//         } else {
//             None
//         };

//         final_paths.push((last, paths[i].clone(), next));
//     }

//     for entry in &final_paths {
//         println!("{}, {}, {}", entry.0.is_some(), entry.1.display(), entry.2.is_some());
//     }

//     return final_paths;
// }


// fn get_paths(directory: &str) -> Vec<PathBuf> {
//     let mut final_paths: Vec<PathBuf> = Vec::new();

//     let paths = {
//         let mut paths: Vec<PathBuf> = Vec::new();

//         for entry in fs::read_dir(directory).unwrap() {
//             paths.push(entry.unwrap().path().to_path_buf());
//         }
//         paths
//     };

//     for i in 0..paths.len() 
//     {
//         let last = if i == 0 {
//             None
//         } else {
//             Some(paths[i - 1].clone())
//         };

//         let next = if (i + 1) < paths.len() {
//             Some(paths[i + 1].clone())
//         } else {
//             None
//         };

//         final_paths.push(paths[i].clone());
//     }

//     // for entry in &final_paths {
//     //     println!("{}, {}, {}", entry.0.is_some(), entry.1.display(), entry.2.is_some());
//     // }

//     return final_paths;
// }

// fn get_directories(directory: &str) {
//     for entry in std::fs::read_dir(directory).unwrap().into_iter().filter_map(|e| e.into()) {
//         let entry = entry.unwrap();
//     }
// }


// fn get_lesson_paths() -> Vec<PathBuf> {
//     return get_paths("../content/lessons");
// }

// fn get_lesson_code_paths() -> Vec<PathBuf> {
//     let paths = get_paths("../..code/source").into_iter().filter(|p | {
//         p.ends_with("CMakeLists.txt")
//     });

//     return paths.collect();
// }

// #[derive(Clone)]
// enum FileOrEnum
// {
//     File(PathBuf),
//     Folder(PathBuf)
// }


// fn get_assets(asset_dir: &Path) -> Vec<FileOrEnum>
// {
//     let mut assets : Vec<FileOrEnum> = Vec::new();

//     let walkdir = WalkDir::new(asset_dir);
//     let it = &mut walkdir
//         .into_iter()
//         .filter_map(|e| e.ok());

//     for entry in it {
//         let entry_path_buf = entry.into_path();
//         let entry_path = entry_path_buf.strip_prefix(&asset_dir).unwrap();

//         if entry_path.as_os_str().is_empty() {
//             continue;
//         }

//         if entry_path.starts_with("Shaders") {
//             continue;
//         }

//         if entry_path_buf.is_file() {
//             assets.push(FileOrEnum::File((entry_path.to_path_buf())));
//         } else if entry_path_buf.is_dir() {
//             assets.push(FileOrEnum::Folder((entry_path.to_path_buf())));
//         }
//     }

//     return assets;
// }


fn get_folders_or_paths(asset_dir: &Path, want_dirs: bool) -> Vec<PathBuf>
{
    let mut paths : Vec<PathBuf> = Vec::new();

    let walkdir = WalkDir::new(asset_dir);
    let it = &mut walkdir
        .into_iter()
        .filter_map(|e| e.ok());

    for entry in it {
        let entry_path_buf = entry.into_path();
        let entry_path = entry_path_buf.strip_prefix(&asset_dir).unwrap();

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


    return paths;
}

fn get_folders(asset_dir: &Path) -> Vec<PathBuf> {
    return get_folders_or_paths(asset_dir, true);
}

fn get_files(asset_dir: &Path) -> Vec<PathBuf> {
    return get_folders_or_paths(asset_dir, false);
}

// fn get_files(asset_dir: &Path) -> Vec<FileOrEnum>
// {
//     let mut assets : Vec<FileOrEnum> = Vec::new();

//     let walkdir = WalkDir::new(asset_dir);
//     let it = &mut walkdir
//         .into_iter()
//         .filter_map(|e| e.ok());

//     for entry in it {
//         let entry_path_buf = entry.into_path();
//         let entry_path = entry_path_buf.strip_prefix(&asset_dir).unwrap();

//         if entry_path.as_os_str().is_empty() {
//             continue;
//         }

//         if entry_path_buf.is_file() {
//             assets.push(FileOrEnum::File((entry_path.to_path_buf())));
//         }
//     }

//     return assets;
// }

// struct Lesson {
//     file_name: String,
//     front_matter: Yaml,
//     markdown: String,
//     lesson_code_directory: PathBuf,
//     code_files: Vec<FileOrEnum>,
//     code_assets: Vec<FileOrEnum>
// }


struct LessonCode {
    lesson_name: String,
    lesson_code_directory: PathBuf,
    code_files: Vec<PathBuf>,
    code_assets: Vec<PathBuf>
}


impl fmt::Display for LessonCode {
    // This trait requires `fmt` with this exact signature.
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        // Write strictly the first element into the supplied output
        // stream: `f`. Returns `fmt::Result` which indicates whether the
        // operation succeeded or failed. Note that `write!` uses syntax which
        // is very similar to `println!`.
        //write!(f, "{}", self.0)

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

// impl fmt::Display for Lesson {
//     // This trait requires `fmt` with this exact signature.
//     fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
//         // Write strictly the first element into the supplied output
//         // stream: `f`. Returns `fmt::Result` which indicates whether the
//         // operation succeeded or failed. Note that `write!` uses syntax which
//         // is very similar to `println!`.
//         //write!(f, "{}", self.0)

//         write!(f, "{}", self.front_matter["title"].as_str().unwrap()).unwrap();
//         write!(f, "\n\t{}", self.front_matter["description"].as_str().unwrap()).unwrap();
//         write!(f, "\n\t{}", self.lesson_code_directory.display()).unwrap();

//         write!(f, "\n\tCode Assets").unwrap();
//         for asset in &self.code_assets {
//             match asset {
//                 FileOrEnum::File(file_asset) => {
//                     write!(f, "\n\t\t{}", file_asset.display()).unwrap();
//                 }
//                 FileOrEnum::Folder(fodler_asset) => {
//                     write!(f, "\n\t\t{}", fodler_asset.display()).unwrap();
//                 }
//             }
//         }
        
//         write!(f, "\n\tCode File").unwrap();
//         for asset in &self.code_files {
//             match asset {
//                 FileOrEnum::File(file_asset) => {
//                     write!(f, "\n\t\t{}", file_asset.display()).unwrap();
//                 }
//                 FileOrEnum::Folder(fodler_asset) => {
//                     write!(f, "\n\t\t{}", fodler_asset.display()).unwrap();
//                 }
//             }
//         }

//         write!(f, "\n\n")
//     }
//}

fn get_code_assets_lesson_needs(assets: &Vec<PathBuf>, lesson_c_source_path: &Path) -> Vec<PathBuf> {
    
    let mut assets_lesson_needs : Vec<PathBuf> = Vec::new();
    
    let c_source = std::fs::read_to_string(lesson_c_source_path).unwrap();

    for file_asset in assets {
        let file_name = file_asset.file_name().unwrap();

        if c_source.contains(file_name.to_str().unwrap()) {
            assets_lesson_needs.push(file_asset.clone());
        }
    }
    
    return assets_lesson_needs;
}

// fn get_lesson_data(source_dir: &Path, assets: &Vec<FileOrEnum>) -> Vec<Lesson> {
//     let mut lessons: Vec<Lesson> = Vec::new();
    
//     for (last, path, next) in get_lesson_paths() {
//         let lesson_name = path.file_stem().unwrap().to_owned();
//         let lesson_code_directory = source_dir.join(&lesson_name);
//         let lesson_c_source_path: PathBuf = lesson_code_directory.join(&lesson_name).with_extension("c");

//         let code_assets = get_code_assets_lesson_needs(assets, &lesson_c_source_path);
//         let code_files = get_code_files_from_lesson(&lesson_code_directory);

//         let front_matter_and_markdown: String = std::fs::read_to_string(path.as_path()).unwrap();
        
//         let (front_matter, markdown) = Extractor::new(Splitter::EnclosingLines("---"))
//             .extract(&front_matter_and_markdown);

//         let docs = YamlLoader::load_from_str(&front_matter).unwrap();

//         lessons.push(Lesson { 
//             file_name: path.file_name().unwrap().to_str().unwrap().to_string(),
//             front_matter: docs[0].clone(), 
//             markdown: markdown.to_string(), 
//             lesson_code_directory: lesson_code_directory.to_path_buf(),
//             code_files,
//             code_assets
//         });
//     }

//     lessons.natural_sort_by_key::<str, _, _>(|x| x.file_name.clone());

//     return lessons;
// }

//static CODE_SOURCE_DIR: &str =  "../../code/source";
//static CODE_ASSET_DIR: &str =  "../../code/Assets";
//static CODE_CMAKE_DIR: &str =  "../../code/CMake";

fn get_specific_lesson_code() -> Vec<LessonCode> {
    let source_dir = Path::new(CODE_SOURCE_DIR);
    let code_asset_dir = Path::new(CODE_ASSET_DIR);
    let mut lessons: Vec<LessonCode> = Vec::new();

    let assets = {
        let asset_final_dir = Path::new(code_asset_dir.file_name().unwrap());
        
        get_files(&code_asset_dir)
            .into_iter()
            .map(|i| asset_final_dir.join(i))
            .collect()
    };

    
    for path in get_folders(&source_dir) {
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

    return lessons;
}

fn get_agnostic_lesson_code() -> Vec<PathBuf> {
    let cmake_dir = Path::new(CODE_CMAKE_DIR);
    let cmake_final_dir = Path::new(cmake_dir.file_name().unwrap());
    
    get_files(&cmake_dir)
        .into_iter()
        .map(|i| cmake_final_dir.join(i))
        .collect()
}


// fn get_non_lesson_content() {

// }

// fn create_source_zip_from_lesson(lesson: &Lesson) {
//     //let mut destination_zip_file_path = file_name_no_extension.to_owned().to_str().unwrap().to_owned();
//     //destination_zip_file_path.push_str(".zip");
//     //let destination_zip_file_path = asset_code_dir.join(&destination_zip_file_path);
//     //let destination_zip_file = File::create(&destination_zip_file_path).unwrap();
// }


// fn render_html(html_template: String, markdown: String) {

// }






// fn zip_dir<T>(
//     prefix: &Path,
//     writer: T,
// ) -> anyhow::Result<()>
// where
//     T: Write + Seek,
// {
//     let walkdir = WalkDir::new(&prefix);
//     let it = &mut walkdir.into_iter().filter_map(|e| e.ok());

//     let mut zip = zip::ZipWriter::new(writer);
//     let options = SimpleFileOptions::default()
//         .compression_method(zip::CompressionMethod::Zstd)
//         .unix_permissions(0o755);

//     let prefix = Path::new(prefix);
//     let mut buffer = Vec::new();
//     for entry in it {
//         let path = entry.path();
//         let name = path.strip_prefix(prefix).unwrap();
//         let path_as_string = name
//             .to_str()
//             .map(str::to_owned)
//             .with_context(|| format!("{name:?} Is a Non UTF-8 Path")).unwrap().replace("\\", "/");

//         // Write file or directory explicitly
//         // Some unzip tools unzip files with directory paths correctly, some do not!
//         if path.is_file() {
//             //println!("adding file {path:?} as {name:?} ...");
//             zip.start_file(path_as_string, options)?;
//             let mut f = File::open(path)?;

//             f.read_to_end(&mut buffer)?;
//             zip.write_all(&buffer)?;
//             buffer.clear();
//         } else if !name.as_os_str().is_empty() {
//             // Only if not root! Avoids path spec / warning
//             // and mapname conversion failed error on unzip
//             //println!("adding dir {path_as_string:?} as {name:?} ...");
//             zip.add_directory(path_as_string, options)?;
//         }
//     }
//     zip.finish()?;

//     Ok(())
// }


fn write_lesson_zips(output_dir: &Path) {
    let code_dir = Path::new(CODE_DIR);
    let output_code_dir = output_dir.join("assets").join("code");
    let agnostic_code_for_lessons = get_agnostic_lesson_code();

    fs::create_dir_all(&output_code_dir).unwrap();

    for lesson_code in get_specific_lesson_code() {
        let zip_file_path = output_code_dir.join(lesson_code.lesson_name).with_extension("zip");
        let zip_file = File::create(&zip_file_path).unwrap();
        let mut zip = zip::ZipWriter::new(zip_file);
        let mut buffer = Vec::new();
        
        let options = SimpleFileOptions::default()
            .compression_method(zip::CompressionMethod::Zstd)
            .unix_permissions(0o755);

        let files = {
            let mut files = Vec::new();
            files.extend_from_slice(&lesson_code.code_assets);
            files.extend_from_slice(&agnostic_code_for_lessons);
            files
        };

        for file in &files {
            zip.start_file_from_path(&file, options).unwrap();
            
            let mut f = File::open(code_dir.join(file)).unwrap();
            f.read_to_end(&mut buffer).unwrap();
            zip.write_all(&buffer).unwrap();
            buffer.clear();
        }
        
        for file in &lesson_code.code_files {
            println!("{}", file.display());
            zip.start_file_from_path(&file, options).unwrap();
            
            let mut f = File::open(lesson_code.lesson_code_directory.join(file)).unwrap();
            f.read_to_end(&mut buffer).unwrap();
            zip.write_all(&buffer).unwrap();
            buffer.clear();
        }

        zip.finish().unwrap();
    }
}

fn write_static_data(output_dir: &Path) {
    let static_data_dir = Path::new(STATIC_DATA_DIR);

    for file_source in get_files(static_data_dir) {
        let file_destination = output_dir.join(&file_source);

        println!("static_stuff: {}", file_destination.display());
        fs::create_dir_all(file_destination.parent().unwrap()).unwrap();
        fs::copy(static_data_dir.join(&file_source), &file_destination).unwrap();
    }
}

fn write_site_to_folder() {
    let output_dir = Path::new(OUTPUT_DIR);

    // Delete existing output    
    if fs::exists(&output_dir).unwrap()
    {
        fs::remove_dir_all(&output_dir).unwrap();
    }

    write_static_data(&output_dir);
    write_lesson_zips(&output_dir);
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

    for file_path in get_files(&content_dir) {
        let file_name = file_path.file_name().unwrap().to_str().unwrap().to_string();

        let front_matter_and_markdown: String = std::fs::read_to_string(&content_dir.join(&file_path)).unwrap();
        
        let (front_matter, markdown) = Extractor::new(Splitter::EnclosingLines("---"))
            .extract(&front_matter_and_markdown);

        let docs = YamlLoader::load_from_str(&front_matter).unwrap();

        content.push(Content { 
            file_name: file_name,
            file_path: file_path, 
            front_matter: docs[0].clone(), 
            markdown: markdown.to_string() 
        });
    }

    return content;
}


fn get_collections(content: &Vec<Content>) -> Value {
    let mut collections_to_return: HashMap<String, Vec<&Content>> = HashMap::new();

    for content_file in content {
        let collections = content_file.front_matter["collections"].as_vec();

        let collections = if collections.is_none() {
            continue;
        } else {
            collections.unwrap()
        };

        for collection in collections {
            let collection_name = collection.as_str().unwrap();
            
            println!("\tcollection_name: {}", collection_name);

            if let Some(inner_collection) = collections_to_return.get_mut(collection_name) {
                inner_collection.push(&content_file);
            } else {
                collections_to_return.insert(collection_name.to_string(), Vec::new());
                let inner_collection = collections_to_return.get_mut(collection_name).unwrap();
                inner_collection.push(&content_file);
            }
        }
    }

    let mut collection_map: serde_json::Map<String, Value> = serde_json::Map::new();
    for (name, collection) in collections_to_return {
        collection_map.insert(name.clone(), Value::Array(collection.iter().map(|i| get_content_info(i).into()).collect()));
    }

    return collection_map.into();
}

fn get_content_info(content: &Content) -> serde_json::Map<String, Value> {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    map.insert("file_name".to_string(), Value::String(content.file_name.clone()));
    map.insert("file_path".to_string(), Value::String(content.file_path.to_str().unwrap().to_string()));
    map.insert("title".to_string(), Value::String(content.front_matter["title"].as_str().unwrap().to_string()));
    map.insert("description".to_string(), Value::String(content.front_matter["description"].as_str().unwrap().to_string()));
    map.insert("url".to_string(), Value::String(content.file_path.with_extension("html").to_str().unwrap().to_string()));

    return map;
}

fn get_content_infos(content: &Vec<Content>) -> Value {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    for content_file in content {
        map.insert(content_file.file_name.clone(), get_content_info(content_file).into());
    }

    return Value::Object(map);
}

fn get_template_context(content: &Vec<Content>) -> serde_json::Map<String, Value> {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    map.insert("contents".to_string(), get_content_infos(&content));
    map.insert("collections".to_string(), get_collections(&content));
    return map;
}

//fn to_json_value(yaml: &Yaml) -> Value {
//    let mut map = Map::new();
//
//    for item in yaml. {
//
//    }
//
//    let obj = Value::Object(map);
//}

fn get_specific_content_context(template_context: &serde_json::Map<String, Value>, content: &Content, rendered_html: &String) -> Value {
    let mut map: serde_json::Map<String, Value> = template_context.clone();
    let mut current_content = get_content_info(&content);
    current_content.insert("rendered_html".to_string(), Value::String(rendered_html.clone()));
    map.insert("current_content".to_string(), current_content.into());

    let content_title = content.front_matter["title"].as_str().unwrap().to_string();

    if let Some(item) = get_toc_from_content_html(&content_title, &rendered_html) {
        map.insert("table_of_contents".to_string(), item.into());
    }

    return map.into();
}


pub fn handlebars_escape(data: &str) -> String {
    if data.contains(NO_ESCAPE) {
        return data.to_owned();
    }
    
    return handlebars::html_escape(data);
}
struct TocItem {
    level: i8,
    text: String,
    url: String,
    children: Vec<TocItem>
}



fn get_header_info_from_element(header: &ElementRef<'_>) -> Option<TocItem> {
    let current_level = header
        .value()
        .name.local
        .to_string()
        .split_off(1)
        .parse::<i8>()
        .unwrap();
    
    let header_text = header.text().into_iter().next().unwrap().to_string();

    if header.child_elements().count() != 0 {
        let child = header.child_elements().into_iter().next().unwrap();

        let url = format!("#{}", child.attr("name").unwrap());

        return Some(TocItem{
            level: current_level,
            text: header_text,
            url, 
            children: Vec::new()
        });
    }

    println!("Warning, missing tags: {}", header.html());
    return None;
}


// fn print_root_toc(item: &TocItem) {
//     let base_indentation = "\t".repeat((item.level - 1) as usize);
//     println!("{}{}: {}", base_indentation, item.text, item.url);

//     for child in &item.children {
//         process_root_toc(child);
//     }
// }


fn process_root_toc(item: &TocItem) -> Value {
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

fn get_toc_from_content_html(title: &String, html: &String) -> Option<Value> {
    let mut children_stack: Vec<TocItem> = Vec::new();
    children_stack.push(TocItem { 
        level: 1, 
        text: title.clone(),
        url: "#title".to_string(), 
        children: Vec::new()
    });


    let document: Html = Html::parse_document(&html);
    let h1_selector = Selector::parse("h1, h2, h3, h4, h5, h6").unwrap();
    let mut last_level = 1;

    for header in document.select(&h1_selector) {
        let info = get_header_info_from_element(&header);
        if info.is_none() {
            continue;
        }

        let item = info.unwrap();

        if item.level == 1 {
            panic!("Processing a header with name {} and id {}, this header has a heading of 1, which is disallowed in content. Anything above 1 is allowed. Headers must start at 2, and only increase one at a time.",
                item.text, 
                item.url);
        }

        if (last_level < item.level) && ((last_level + 2) == item.level) { // Starting a new stack.
            last_level += 1;
            children_stack.push(item);
        } else if (last_level < item.level) && ((last_level + 1) == item.level) { // We're one level ahead, just push into the children of the last item of the previous level
            children_stack.iter_mut().nth_back(0).unwrap().children.push(item);
        } else if last_level >= item.level {
            while last_level >= item.level {
                let last_item  = children_stack.pop().unwrap();
                children_stack.iter_mut().nth_back(0).unwrap().children.push(last_item);
                last_level -= 1;
            }

            children_stack.iter_mut().nth_back(0).unwrap().children.push(item);
        }
    }

    while last_level < children_stack.iter_mut().nth_back(0).unwrap().level {
        let last_item  = children_stack.pop().unwrap();
        children_stack.iter_mut().nth_back(0).unwrap().children.push(last_item);
        last_level -= 1;
    }

    if children_stack.len() != 0 {
        return Some(process_root_toc(children_stack.first().unwrap()));
    }

    return None
}


fn process_content() -> Vec<(PathBuf, String)> {
    let output_dir = Path::new(OUTPUT_DIR);

    let template_dir = Path::new(TEMPLATE_DIR);
    let rendered_html: Vec<(PathBuf, String)> = Vec::new();

    let contents = get_content();
    let template_context = get_template_context(&contents);

    let mut handlebars = Handlebars::new();
    handlebars.register_escape_fn(handlebars_escape);

    let options = Options {
            compile: CompileOptions {
            allow_dangerous_html: true,
            ..CompileOptions::default()
        },
        ..Options::gfm()
    };

    for content in contents {
        let template_path = template_dir.join(content.front_matter["template"].as_str().unwrap());
        let template_html = std::fs::read_to_string(&template_path).unwrap();
        let content_html = format!("{}{}", NO_ESCAPE, markdown::to_html_with_options(&content.markdown, &options).unwrap());


//Option<Value>
        let current_content_context = get_specific_content_context(&template_context, &content, &content_html);
        let final_html = handlebars.render_template(&template_html, &current_content_context).unwrap();

        let final_file_path = output_dir.join(&content.file_path).with_extension("html");
        create_dir_all(final_file_path.parent().unwrap()).unwrap();

        std::fs::write(final_file_path, final_html).unwrap();
    }

    return rendered_html;
}

fn main() {
    let output_dir = Path::new(OUTPUT_DIR);
    write_site_to_folder();
    process_content();

   let args: Vec<String> = env::args().collect();

   if !args.contains(&"--no-serve".to_owned())
   {
       // Should run the bottom command to host the site.
       let _ = Command::new("http-serve-folder")
           .args([output_dir])
           .status()
           .expect("failed to execute process");
   }
}


//
//fn main() {
//
//    let args: Vec<String> = env::args().collect();
//
//
//    //let site_dir = Path::new("../");
//    //let template_dir = Path::new("../template/");
//    //let output_dir = Path::new("output");
//    //let asset_dir = output_dir.join("assets");
//    //let asset_code_dir = asset_dir.join("code");
//    //let asset_image_dir = asset_dir.join("images");
//
//    let code_dir = Path::new("../../code");
//    let source_dir = code_dir.join("source");
//    let code_asset_dir = code_dir.join("Assets");
//
//    let lessons = get_lesson_data(&source_dir, &get_assets(&code_asset_dir));
//
//    for lesson in lessons {
//        println!("{}", lesson);
//    }
//
//    /*
//    if fs::exists(&output_dir).unwrap()
//    {
//        fs::remove_dir_all(&output_dir).unwrap();
//    }
//    fs::create_dir(&output_dir).unwrap();
//    copy_dir_all(site_dir.join("assets"), output_dir.join("assets")).unwrap();
//    fs::create_dir(&asset_code_dir).unwrap();
//    copy_dir_all(template_dir.join("css"), output_dir.join("css")).unwrap();
//    copy_dir_all(template_dir.join("fonts"), output_dir.join("fonts")).unwrap();
//    copy_dir_all(template_dir.join("js"), output_dir.join("js")).unwrap();
//
//    let post_template = std::fs::read_to_string("../template/post_template.html").unwrap();
//    let paths = get_lesson_paths();
//
//    let mut posts = Vec::new();
//
//    for (last, path, next) in paths {
//        let file_name_no_extension = path.file_stem().unwrap().to_owned();
//
//        let mut output_file=  file_name_no_extension.clone();
//        output_file.push(".html");
//        let output_file = output_dir.join(output_file);
//        let file_name = path.file_name().unwrap().to_str().unwrap();
//
//        let mut data: String = std::fs::read_to_string(path.as_path()).unwrap();
//
//        {
//            let mut destination_zip_file_path = file_name_no_extension.to_owned().to_str().unwrap().to_owned();
//            destination_zip_file_path.push_str(".zip");
//            let destination_zip_file_path = asset_code_dir.join(&destination_zip_file_path);
//            let destination_zip_file = File::create(&destination_zip_file_path).unwrap();
//
//            let example_source_dir = source_dir.join(&file_name_no_extension);
//            zip_dir(&example_source_dir, destination_zip_file, ).unwrap();
//
//            let zip_url = destination_zip_file_path
//                .strip_prefix(output_dir)
//                .unwrap()
//                .to_str()
//                .unwrap()
//                .to_owned()
//                .replace("\\", "/");
//            
//            data.push_str(&format!("\n\n[Download the example code here.]({})", &zip_url));
//        };
//
//
//        let post = parse_post(&file_name, &data, &post_template);
//
//        std::fs::write(output_file, &post.content).unwrap();
//
//
//
//        posts.push(post);
//    }
//
//    posts.natural_sort_by_key::<str, _, _>(|x| x.url.clone());
//
//    let index_page = std::fs::read_to_string(template_dir.join("index_template.html"))
//        .unwrap()
//        .replace("<<<<<<<<POST_CONTENT>>>>>>>>", &get_index_entries(&posts));
//
//    std::fs::write(output_dir.join("index.html"), &index_page).unwrap();
//    std::fs::copy(template_dir.join("about.html"), output_dir.join("about.html")).unwrap();
//
//    if !args.contains(&"--no-serve".to_owned())
//    {
//        // Should run the bottom command to host the site.
//        let _ = Command::new("http-serve-folder")
//            .args([output_dir])
//            .status()
//            .expect("failed to execute process");
//    }
//     */
//}
//