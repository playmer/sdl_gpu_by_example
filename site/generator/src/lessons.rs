
use std::{fmt, fs};
use std::io::stdout;
use std::io::{Read, Write};
use std::path::{Path, PathBuf};
use std::thread;

use natural_sort_rs::NaturalSort;

use zip::write::SimpleFileOptions;

use crate::config;
use crate::fs_utils;



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
    let source_dir = Path::new(config::CODE_SOURCE_DIR);
    let code_asset_dir = Path::new(config::CODE_ASSET_DIR);
    let mut lessons: Vec<LessonCode> = Vec::new();

    let assets = {
        let asset_final_dir = Path::new(code_asset_dir.file_name().unwrap());
        
        fs_utils::get_files(code_asset_dir)
            .into_iter()
            .map(|i| asset_final_dir.join(i))
            .collect()
    };

    
    for path in fs_utils::get_folders(source_dir) {
        let lesson_name = path;
        let lesson_code_directory = source_dir.join(&lesson_name);
        let lesson_c_source_path: PathBuf = lesson_code_directory.join(&lesson_name).with_extension("c");

        let code_assets = get_code_assets_lesson_needs(&assets, &lesson_c_source_path);
        let code_files = fs_utils::get_files(&lesson_code_directory);

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
    let cmake_dir = Path::new(config::CODE_CMAKE_DIR);
    let cmake_final_dir = Path::new(cmake_dir.file_name().unwrap());
    
    fs_utils::get_files(cmake_dir)
        .into_iter()
        .map(|i| cmake_final_dir.join(i))
        .collect()
}


pub fn write_lesson_zips(output_dir: &Path) {
    println!("Writing lesson zips");

    let code_dir = Path::new(config::CODE_DIR);
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
            let zip_file = fs::File::create(&zip_file_path).unwrap();
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
                
                let mut f = fs::File::open(code_dir.join(file)).unwrap();
                f.read_to_end(&mut buffer).unwrap();
                zip.write_all(&buffer).unwrap();
                buffer.clear();
            }
            
            for file in &lesson_code.code_files {
                println!("\t{}", file.display());
                zip.start_file_from_path(file, options).unwrap();
                
                let mut f = fs::File::open(lesson_code.lesson_code_directory.join(file)).unwrap();
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
