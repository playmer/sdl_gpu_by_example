
use std::collections::HashMap;
use std::fs;
use std::io::stdout;
use std::io::Write;
use std::path::{Path, PathBuf};

use extract_frontmatter::config::Splitter;
use extract_frontmatter::Extractor;
use serde_json::Value;
use yaml_rust::{Yaml, YamlLoader};

use crate::config;
use crate::fs_utils;

pub struct Content {
    pub file_name: String,
    pub file_path: PathBuf,
    pub front_matter: Yaml,
    pub markdown: String,
}


pub fn get_content() -> Vec<Content> {
    let content_dir = Path::new(config::CONTENT_DIR);

    let mut content: Vec<Content> = Vec::new();

    for file_path in fs_utils::get_files(content_dir) {
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


pub fn get_collections(content: &Vec<Content>) -> Value {
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

pub fn get_content_info(content: &Content) -> serde_json::Map<String, Value> {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    map.insert("file_name".to_string(), Value::String(content.file_name.clone()));
    map.insert("file_name_no_ext".to_string(), Value::String(content.file_path.file_stem().unwrap().to_str().unwrap().to_string()));
    map.insert("file_path".to_string(), Value::String(content.file_path.to_str().unwrap().to_string()));
    map.insert("title".to_string(), Value::String(content.front_matter["title"].as_str().unwrap().to_string()));
    map.insert("description".to_string(), Value::String(content.front_matter["description"].as_str().unwrap().to_string()));

    if let Some(status) = content.front_matter["example_status"].as_str() {
        map.insert("example_status".to_string(), Value::String(status.to_string()));
    }
    
    if let Some(status) = content.front_matter["chapter_status"].as_str() {
        map.insert("chapter_status".to_string(), Value::String(status.to_string()));
    }

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

pub fn get_template_context(content: &Vec<Content>) -> serde_json::Map<String, Value> {
    let mut map: serde_json::Map<String, Value> = serde_json::Map::new();

    map.insert("contents".to_string(), get_content_infos(content));
    map.insert("collections".to_string(), get_collections(content));
    map
}


pub fn get_inserts() -> Vec<(String, String)> {
    let inserts_dir = Path::new("../inserts");
    let mut inserts = Vec::new();

    for insert in fs_utils::get_files(inserts_dir) {
        let name = insert.file_stem().unwrap().to_str().unwrap().to_string();
        let insert_html = fs::read_to_string(inserts_dir.join(insert)).unwrap();

        inserts.push((name, insert_html));
    }

    inserts
}
