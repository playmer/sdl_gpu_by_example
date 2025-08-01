use core::panic;
use std::fs::{DirEntry, File};
use std::io::{Read, Seek, Write};
use std::path::{Path, PathBuf};
use std::process::Command;
use std::{env, fmt, fs, io};
use anyhow::Context;
use extract_frontmatter::config::{Modifier, Splitter};
use extract_frontmatter::Extractor;
use markdown::{to_mdast, Constructs, Options, ParseOptions};
use markdown;
use natural_sort_rs::NaturalSort;
use walkdir::WalkDir;
use yaml_rust::{Yaml, YamlLoader};
use zip::write::SimpleFileOptions;


fn copy_dir_all(src: impl AsRef<Path>, dst: impl AsRef<Path>) -> io::Result<()> {
    fs::create_dir_all(&dst)?;
    for entry in fs::read_dir(src)? {
        let entry = entry?;
        let ty = entry.file_type()?;
        if ty.is_dir() {
            copy_dir_all(entry.path(), dst.as_ref().join(entry.file_name()))?;
        } else {
            fs::copy(entry.path(), dst.as_ref().join(entry.file_name()))?;
        }
    }
    Ok(())
}

struct Post {
    title: String,
    description: String, 
    url: String,
    content: String
}

fn parse_post(file_name: &str, markdown_data: &String, post_template: &str) -> Post {
    let front_matter = Options {
        parse: ParseOptions {
            constructs: Constructs {
                frontmatter: true,
                ..Default::default()
            },
            ..Default::default()
        },
        ..Default::default()
    };

    let ast = to_mdast(markdown_data, &front_matter.parse).unwrap();

    let mut title = String::new();
    let mut description = String::new();

    let yaml = ast.children().unwrap()[0].to_string();
    let yaml : Vec<&str> = yaml.split('\n').collect();
    for entry in yaml {
        let entry = entry.trim();
        if entry.starts_with("title:") {
            let mut splits : Vec<&str> = entry.split("title:").collect();
            splits.retain(|&x| !x.trim().is_empty());
            
            //println!("hit title: [{}]", splits[0]);
            title = splits[0].trim().to_owned();
        }
        else if entry.starts_with("description:") {
            let mut splits : Vec<&str> = entry.split("description:").collect();
            splits.retain(|&x| !x.trim().is_empty());
            //println!("hit description: {}", splits[0]);
            description = splits[0].trim().to_owned();
        }
    }

    if title.len() == 0 {
        panic!("Couldn't find a title for {file_name}");
    }
    
    if description.len() == 0 {
        panic!("Couldn't find a description for {file_name}");
    }
    
    let post_content = markdown::to_html_with_options(&markdown_data, &front_matter).unwrap();

    return Post{
        title: title.clone(),
        description,
        url: file_name.replace("md", "html"),
        content: post_template
        .replace("<<<<<<<<POST_TITLE>>>>>>>>", &title)
        .replace("<<<<<<<<POST_CONTENT>>>>>>>>", &post_content)
    };
}


// Need to figure out a way to dump images into the html like this
const IMAGE_TEMPLATE: &str = "
<p>
  <a href=\"assets/images/002_Window_and_Clearing__Running.jpg\" target=\"_blank\">
    <img src=\"assets/images/002_Window_and_Clearing__Running.jpg\" style='width:100%;' border=\"0\" alt=\"Null\">
  </a>
</p>
";

const INDEX_ENTRY : &str = "

<h4 class=\"<<<<<<<<POST_TITLE>>>>>>>>\">
<a href=\"<<<<<<<<POST_URL>>>>>>>>\"><<<<<<<<POST_TITLE>>>>>>>></a>
</h4>
<p><<<<<<<<POST_DESCRIPTION>>>>>>>></p>

<hr>

";

fn get_index_entries(posts: &Vec<Post>) -> String
{
    let mut index_content = String::new();

    for (i, post) in posts.iter().enumerate()
    {
        index_content += &INDEX_ENTRY
            .replace("<<<<<<<<POST_TITLE>>>>>>>>", &format!("{}. {}", i + 1, post.title))
            .replace("<<<<<<<<POST_URL>>>>>>>>", &post.url)
            .replace("<<<<<<<<POST_DESCRIPTION>>>>>>>>", &post.description);
    }

    return index_content;
}

fn zip_dir<T>(
    prefix: &Path,
    writer: T,
) -> anyhow::Result<()>
where
    T: Write + Seek,
{
    let walkdir = WalkDir::new(&prefix);
    let it = &mut walkdir.into_iter().filter_map(|e| e.ok());

    let mut zip = zip::ZipWriter::new(writer);
    let options = SimpleFileOptions::default()
        .compression_method(zip::CompressionMethod::Zstd)
        .unix_permissions(0o755);

    let prefix = Path::new(prefix);
    let mut buffer = Vec::new();
    for entry in it {
        let path = entry.path();
        let name = path.strip_prefix(prefix).unwrap();
        let path_as_string = name
            .to_str()
            .map(str::to_owned)
            .with_context(|| format!("{name:?} Is a Non UTF-8 Path")).unwrap().replace("\\", "/");

        // Write file or directory explicitly
        // Some unzip tools unzip files with directory paths correctly, some do not!
        if path.is_file() {
            //println!("adding file {path:?} as {name:?} ...");
            zip.start_file(path_as_string, options)?;
            let mut f = File::open(path)?;

            f.read_to_end(&mut buffer)?;
            zip.write_all(&buffer)?;
            buffer.clear();
        } else if !name.as_os_str().is_empty() {
            // Only if not root! Avoids path spec / warning
            // and mapname conversion failed error on unzip
            //println!("adding dir {path_as_string:?} as {name:?} ...");
            zip.add_directory(path_as_string, options)?;
        }
    }
    zip.finish()?;

    Ok(())
}


fn get_lesson_paths() -> Vec<(Option<PathBuf>, PathBuf, Option<PathBuf>)> {
    let mut final_paths: Vec<(Option<PathBuf>, PathBuf, Option<PathBuf>)> = Vec::new();

    let paths = {
        let mut paths: Vec<PathBuf> = Vec::new();

        for entry in fs::read_dir("../content/lessons").unwrap() {
            paths.push(entry.unwrap().path().to_path_buf());
        }
        paths
    };

    for i in 0..paths.len() 
    {
        let last = if i == 0 {
            None
        } else {
            Some(paths[i - 1].clone())
        };

        let next = if (i + 1) < paths.len() {
            Some(paths[i + 1].clone())
        } else {
            None
        };

        final_paths.push((last, paths[i].clone(), next));
    }

    for entry in &final_paths {
        println!("{}, {}, {}", entry.0.is_some(), entry.1.display(), entry.2.is_some());
    }

    return final_paths;
}

#[derive(Clone)]
enum FileOrEnum
{
    File(PathBuf),
    Folder(PathBuf)
}


fn get_assets(asset_dir: &Path) -> Vec<FileOrEnum>
{
    let mut assets : Vec<FileOrEnum> = Vec::new();

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

        if entry_path.starts_with("Shaders") {
            continue;
        }

        if entry_path_buf.is_file() {
            //println!("File: {}", entry_path.display());
            assets.push(FileOrEnum::File((entry_path.to_path_buf())));
        } else if entry_path_buf.is_dir() {
            //println!("Folder: {}", entry_path.display());
            assets.push(FileOrEnum::Folder((entry_path.to_path_buf())));
        }
    }

    return assets;
}

struct Lesson {
    front_matter: Yaml,
    markdown: String,
    lesson_code_directory: PathBuf,
    code_files: Vec<FileOrEnum>,
    code_assets: Vec<FileOrEnum>
}

impl fmt::Display for Lesson {
    // This trait requires `fmt` with this exact signature.
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        // Write strictly the first element into the supplied output
        // stream: `f`. Returns `fmt::Result` which indicates whether the
        // operation succeeded or failed. Note that `write!` uses syntax which
        // is very similar to `println!`.
        //write!(f, "{}", self.0)

        write!(f, "{}", self.front_matter["title"].as_str().unwrap()).unwrap();
        write!(f, "\n\t{}", self.front_matter["description"].as_str().unwrap()).unwrap();
        write!(f, "\n\t{}", self.lesson_code_directory.display()).unwrap();

        write!(f, "\n\tCode Assets").unwrap();
        for asset in &self.code_assets {
            match asset {
                FileOrEnum::File(file_asset) => {
                    write!(f, "\n\t\t{}", file_asset.display()).unwrap();
                }
                FileOrEnum::Folder(fodler_asset) => {
                    write!(f, "\n\t\t{}", fodler_asset.display()).unwrap();
                }
            }
        }
        
        write!(f, "\n\tCode File").unwrap();
        for asset in &self.code_files {
            match asset {
                FileOrEnum::File(file_asset) => {
                    write!(f, "\n\t\t{}", file_asset.display()).unwrap();
                }
                FileOrEnum::Folder(fodler_asset) => {
                    write!(f, "\n\t\t{}", fodler_asset.display()).unwrap();
                }
            }
        }

        write!(f, "\n\n")
    }
}

fn get_code_files_from_lesson(lesson_path: &Path) -> Vec<FileOrEnum>
{
    //println!("prefix: {}", lesson_path.display());
    let mut other_files : Vec<FileOrEnum> = Vec::new();

    let walkdir = WalkDir::new(&lesson_path);
    let it = &mut walkdir
        .into_iter()
        .filter_map(|e| e.ok());

    for entry in it {
        let entry_path_buf = entry.into_path();
        //println!("\tentry: {}", entry_path_buf.display());

        let entry_path = entry_path_buf.strip_prefix(&lesson_path).unwrap();

        if entry_path.as_os_str().is_empty() {
            continue;
        }


        if entry_path_buf.is_file() {
            //println!("\t\tFile: {}", entry_path.display());
            other_files.push(FileOrEnum::File((entry_path.to_path_buf())));
        } else if entry_path_buf.is_dir() {
            //println!("\t\tFolder: {}", entry_path.display());
            other_files.push(FileOrEnum::Folder((entry_path.to_path_buf())));
        }
    }

    return other_files;
}

fn get_code_assets_lesson_needs(assets: &Vec<FileOrEnum>, lesson_c_source_path: &Path) -> Vec<FileOrEnum> {
    
    let mut assets_lesson_needs : Vec<FileOrEnum> = Vec::new();
    
    let c_source = std::fs::read_to_string(lesson_c_source_path).unwrap();
    
    for asset in assets{
        match asset {
            FileOrEnum::File(file_asset) => {
                let file_name = file_asset.file_name().unwrap();
                if c_source.contains(file_name.to_str().unwrap()) {
                    assets_lesson_needs.push(asset.clone());
                }
            }
            _ => {}
        }
    }
    
    //let destination_zip_file = File::create(&destination_zip_file_path).unwrap();
    //let mut zip = zip::ZipWriter::new(writer);
    //let options = SimpleFileOptions::default()
    //    .compression_method(zip::CompressionMethod::Zstd)
    //    .unix_permissions(0o755);


    return assets_lesson_needs;
}

fn get_lesson_data(source_dir: &Path, assets: &Vec<FileOrEnum>) -> Vec<Lesson> {
    let mut lessons: Vec<Lesson> = Vec::new();
    
    for (last, path, next) in get_lesson_paths() {
        let lesson_name = path.file_stem().unwrap().to_owned();
        let lesson_code_directory = source_dir.join(&lesson_name);
        let lesson_c_source_path: PathBuf = lesson_code_directory.join(&lesson_name).with_extension("c");

        let code_assets = get_code_assets_lesson_needs(assets, &lesson_c_source_path);
        let code_files = get_code_files_from_lesson(&lesson_code_directory);

        let front_matter_and_markdown: String = std::fs::read_to_string(path.as_path()).unwrap();
        
        let (front_matter, markdown) = Extractor::new(Splitter::EnclosingLines("---"))
            .extract(&front_matter_and_markdown);

        let docs = YamlLoader::load_from_str(&front_matter).unwrap();

        lessons.push(Lesson { 
            front_matter: docs[0].clone(), 
            markdown: markdown.to_string(), 
            lesson_code_directory: lesson_code_directory.to_path_buf(),
            code_files,
            code_assets
        });
    }

    return lessons;
}



fn get_non_lesson_content() {

}

fn create_source_zip_from_lesson(lesson: &Lesson) {
    //let mut destination_zip_file_path = file_name_no_extension.to_owned().to_str().unwrap().to_owned();
    //destination_zip_file_path.push_str(".zip");
    //let destination_zip_file_path = asset_code_dir.join(&destination_zip_file_path);
    //let destination_zip_file = File::create(&destination_zip_file_path).unwrap();
}



fn main() {

    let args: Vec<String> = env::args().collect();


    //let site_dir = Path::new("../");
    //let template_dir = Path::new("../template/");
    //let output_dir = Path::new("output");
    //let asset_dir = output_dir.join("assets");
    //let asset_code_dir = asset_dir.join("code");
    //let asset_image_dir = asset_dir.join("images");

    let code_dir = Path::new("../../code");
    let source_dir = code_dir.join("source");
    let code_asset_dir = code_dir.join("Assets");

    let lessons = get_lesson_data(&source_dir, &get_assets(&code_asset_dir));

    for lesson in lessons {
        println!("{}", lesson);
    }

    /*
    if fs::exists(&output_dir).unwrap()
    {
        fs::remove_dir_all(&output_dir).unwrap();
    }
    fs::create_dir(&output_dir).unwrap();
    copy_dir_all(site_dir.join("assets"), output_dir.join("assets")).unwrap();
    fs::create_dir(&asset_code_dir).unwrap();
    copy_dir_all(template_dir.join("css"), output_dir.join("css")).unwrap();
    copy_dir_all(template_dir.join("fonts"), output_dir.join("fonts")).unwrap();
    copy_dir_all(template_dir.join("js"), output_dir.join("js")).unwrap();

    let post_template = std::fs::read_to_string("../template/post_template.html").unwrap();
    let paths = get_lesson_paths();

    let mut posts = Vec::new();

    for (last, path, next) in paths {
        let file_name_no_extension = path.file_stem().unwrap().to_owned();

        let mut output_file=  file_name_no_extension.clone();
        output_file.push(".html");
        let output_file = output_dir.join(output_file);
        let file_name = path.file_name().unwrap().to_str().unwrap();

        let mut data: String = std::fs::read_to_string(path.as_path()).unwrap();

        {
            let mut destination_zip_file_path = file_name_no_extension.to_owned().to_str().unwrap().to_owned();
            destination_zip_file_path.push_str(".zip");
            let destination_zip_file_path = asset_code_dir.join(&destination_zip_file_path);
            let destination_zip_file = File::create(&destination_zip_file_path).unwrap();

            let example_source_dir = source_dir.join(&file_name_no_extension);
            zip_dir(&example_source_dir, destination_zip_file, ).unwrap();

            let zip_url = destination_zip_file_path
                .strip_prefix(output_dir)
                .unwrap()
                .to_str()
                .unwrap()
                .to_owned()
                .replace("\\", "/");
            
            data.push_str(&format!("\n\n[Download the example code here.]({})", &zip_url));
        };


        let post = parse_post(&file_name, &data, &post_template);

        std::fs::write(output_file, &post.content).unwrap();



        posts.push(post);
    }

    posts.natural_sort_by_key::<str, _, _>(|x| x.url.clone());

    let index_page = std::fs::read_to_string(template_dir.join("index_template.html"))
        .unwrap()
        .replace("<<<<<<<<POST_CONTENT>>>>>>>>", &get_index_entries(&posts));

    std::fs::write(output_dir.join("index.html"), &index_page).unwrap();
    std::fs::copy(template_dir.join("about.html"), output_dir.join("about.html")).unwrap();

    if !args.contains(&"--no-serve".to_owned())
    {
        // Should run the bottom command to host the site.
        let _ = Command::new("http-serve-folder")
            .args([output_dir])
            .status()
            .expect("failed to execute process");
    }
     */
}
