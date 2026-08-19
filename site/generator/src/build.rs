use std::path::Path;

use std::thread;
use std::fs;

use crate::config;
use crate::lessons;
use crate::templates;
use crate::static_files;

pub fn build_site()
{
    //diff::diff_and_highlight();

    let output_dir = Path::new(config::OUTPUT_DIR);

    // Delete existing output    
    if fs::exists(output_dir).unwrap()
    {
        fs::remove_dir_all(output_dir).unwrap();
    }

    let lesson_zip_task = thread::spawn(move || {
        lessons::write_lesson_zips(output_dir);
    });

    let static_data_task = thread::spawn(move || {
        static_files::write_static_data(output_dir);
    });
    
    static_data_task.join().unwrap();
    lesson_zip_task.join().unwrap();

    templates::process_content();
}