use std::io::stdout;
use std::io::{Read, Write};
use std::path::{Path, PathBuf};
use std::thread;
use std::{fmt, fs};

use anyhow::Context;
use natural_sort_rs::NaturalSort;

use zip::write::SimpleFileOptions;

use crate::config::BuildConfig;
use crate::fs_utils;

struct LessonCode {
    lesson_name: String,
    lesson_code_directory: PathBuf,
    code_files: Vec<PathBuf>,
    code_assets: Vec<PathBuf>,
}

impl fmt::Display for LessonCode {
    // This trait requires `fmt` with this exact signature.
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{}", self.lesson_name)?;
        write!(f, "\n\t{}", self.lesson_code_directory.display())?;

        write!(f, "\n\tCode Assets")?;
        for asset in &self.code_assets {
            write!(f, "\n\t\t{}", asset.display())?;
        }

        write!(f, "\n\tCode File")?;
        for asset in &self.code_files {
            write!(f, "\n\t\t{}", asset.display())?;
        }

        write!(f, "\n\n")
    }
}

fn get_code_assets_lesson_needs(
    assets: &Vec<PathBuf>,
    lesson_c_source_path: &Path,
) -> anyhow::Result<Vec<PathBuf>> {
    let mut assets_lesson_needs: Vec<PathBuf> = Vec::new();

    let c_source = std::fs::read_to_string(lesson_c_source_path).with_context(|| {
        format!(
            "Lesson {} can't be read, is this an empty Lesson Directory?",
            lesson_c_source_path.display()
        )
    })?;

    for file_asset in assets {
        let file_name = file_asset
            .file_name()
            .with_context(|| {
                format!(
                    "Code asset file name {} for lesson {} can't be converted to a utf8 string.",
                    file_asset.display(),
                    lesson_c_source_path.display()
                )
            })?
            .to_str()
            .with_context(|| {
                format!(
                    "Code asset file name {} for lesson {} can't be converted to a utf8 string.",
                    file_asset.display(),
                    lesson_c_source_path.display()
                )
            })?;

        if c_source.contains(file_name) {
            assets_lesson_needs.push(file_asset.clone());
        }
    }

    Ok(assets_lesson_needs)
}

fn get_specific_lesson_code(config: &BuildConfig) -> anyhow::Result<Vec<LessonCode>> {
    let source_dir = &config.code_source_dir;
    let code_asset_dir = &config.code_asset_dir;
    let mut lessons: Vec<LessonCode> = Vec::new();

    let assets = {
        let asset_final_dir = code_asset_dir
            .file_name()
            .with_context(|| format!("{} doesn't have a directory name.", code_asset_dir.display()))?;
        let asset_final_dir = Path::new(asset_final_dir);

        fs_utils::get_files(code_asset_dir)?
            .into_iter()
            .map(|i| asset_final_dir.join(i))
            .collect()
    };

    for path in fs_utils::get_folders(source_dir)? {
        let lesson_name = path;
        let lesson_code_directory = source_dir.join(&lesson_name);
        let lesson_c_source_path: PathBuf =
            lesson_code_directory.join(&lesson_name).with_extension("c");

        let code_assets = get_code_assets_lesson_needs(&assets, &lesson_c_source_path)?;
        let code_files = fs_utils::get_files(&lesson_code_directory)?;
        let lesson_name = lesson_name
            .to_str()
            .with_context(|| format!("{} cant be converted to utf8.", lesson_name.display()))
            ?.to_string();

        lessons.push(LessonCode {
            lesson_name: lesson_name,
            lesson_code_directory: lesson_code_directory.to_path_buf(),
            code_files,
            code_assets,
        });
    }

    lessons.natural_sort_by_key::<str, _, _>(|x| x.lesson_name.clone());

    Ok(lessons)
}

fn get_agnostic_lesson_code(config: &BuildConfig) -> anyhow::Result<Vec<PathBuf>> {
    let cmake_dir = &config.code_cmake_dir.file_name().expect(&format!(
        "{} should have a leaf file name to retrieve.",
        config.code_cmake_dir.display()
    ));
    let cmake_final_dir = Path::new(cmake_dir);

    Ok(fs_utils::get_files(&config.code_cmake_dir)?
        .into_iter()
        .map(|i| cmake_final_dir.join(i))
        .collect())
}

fn lesson_writing_worker(
    lesson_code: LessonCode,
    code_dir: PathBuf,
    output_code_dir: PathBuf,
    agnostic_code_for_lessons: Vec<PathBuf>,
) -> anyhow::Result<()> {
    let zip_file_path = output_code_dir
        .join(lesson_code.lesson_name)
        .with_extension("zip");
    let zip_file = fs::File::create(&zip_file_path)
        .with_context(|| format!("Failed to create file {}.", zip_file_path.display()))?;
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
        zip.start_file_from_path(file, options)?;

        let mut f = fs::File::open(code_dir.join(file))?;
        f.read_to_end(&mut buffer)?;
        zip.write_all(&buffer)?;
        buffer.clear();
    }

    for file in &lesson_code.code_files {
        println!("\t{}", file.display());
        zip.start_file_from_path(file, options)?;

        let mut f = fs::File::open(lesson_code.lesson_code_directory.join(file))?;
        f.read_to_end(&mut buffer)?;
        zip.write_all(&buffer)?;
        buffer.clear();
    }

    zip.finish()?;

    println!("Finished zipping {}", zip_file_path.as_path().display());
    stdout().flush()?;
    Ok(())
}

pub fn write_lesson_zips(config: &BuildConfig) -> anyhow::Result<()> {
    println!("Writing lesson zips");

    let code_dir = config.code_dir.clone();
    let output_code_dir = config.output_dir.join("assets").join("code");
    let agnostic_code_for_lessons = get_agnostic_lesson_code(config)?;

    fs::create_dir_all(&output_code_dir)
        .with_context(|| format!("Failed to create directory {}.", output_code_dir.display()))?;

    let mut handles: Vec<thread::JoinHandle<anyhow::Result<()>>> = Vec::new();

    for lesson_code in get_specific_lesson_code(config)? {
        // Clones for the thread we're spawning.
        let code_dir = code_dir.clone();
        let output_code_dir = output_code_dir.clone();
        let agnostic_code_for_lessons = agnostic_code_for_lessons.clone();

        handles.push(thread::spawn(move || -> anyhow::Result<()> {
            lesson_writing_worker(
                lesson_code,
                code_dir,
                output_code_dir,
                agnostic_code_for_lessons,
            )
        }));
    }

    let results: Vec<_> = handles.into_iter().map(|handle| handle.join()).collect();

    for result in results {
        result.map_err(|_| anyhow::anyhow!("lesson ZIP worker panicked"))??;
    }

    Ok(())
}
