use core::panic;
use std::fs::File;
use std::io::{Read, Seek, Write};
use std::path::Path;
use std::process::Command;
use std::{env, fs, io};
use anyhow::Context;
use markdown::{to_mdast, Constructs, Options, ParseOptions};
use markdown;
use natural_sort_rs::NaturalSort;
use walkdir::WalkDir;
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

fn main() {
    let args: Vec<String> = env::args().collect();
    let template_dir = Path::new("../template/");
    let output_dir = Path::new("output");
    let asset_dir = output_dir.join("assets");
    let asset_code_dir = asset_dir.join("code");
    let asset_image_dir = asset_dir.join("images");
    let source_dir = Path::new("../../code/source");

    if fs::exists(&output_dir).unwrap()
    {
        fs::remove_dir_all(&output_dir).unwrap();
    }
    fs::create_dir(&output_dir).unwrap();
    fs::create_dir(&asset_dir).unwrap();
    fs::create_dir(&asset_code_dir).unwrap();
    fs::create_dir(&asset_image_dir).unwrap();
    copy_dir_all(template_dir.join("css"), output_dir.join("css")).unwrap();
    copy_dir_all(template_dir.join("fonts"), output_dir.join("fonts")).unwrap();
    copy_dir_all(template_dir.join("js"), output_dir.join("js")).unwrap();

    let post_template = std::fs::read_to_string("../template/post_template.html").unwrap();
    let paths = fs::read_dir("../content/lessons").unwrap();

    let mut posts = Vec::new();

    for path in paths {
        let path = path.unwrap();
        let file_name_no_extension = path.path().file_stem().unwrap().to_owned();



        let mut output_file=  file_name_no_extension.clone();
        output_file.push(".html");
        let output_file = output_dir.join(output_file);
        let file_name = path.file_name().into_string().unwrap();

        let mut data: String = std::fs::read_to_string(path.path()).unwrap();

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
    
}
