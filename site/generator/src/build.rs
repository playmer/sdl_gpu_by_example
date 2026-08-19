use std::fs;
use std::thread;

use crate::config::BuildConfig;
use crate::lessons;
use crate::static_files;
use crate::templates;

pub fn build_site(config: &BuildConfig) {
    // Delete existing output
    if fs::exists(&config.output_dir).unwrap() {
        fs::remove_dir_all(&config.output_dir).unwrap();
    }

    let lesson_config = config.clone();
    let lesson_zip_task = thread::spawn(move || {
        lessons::write_lesson_zips(&lesson_config);
    });

    let static_data_config = config.clone();
    let static_data_task = thread::spawn(move || {
        static_files::write_static_data(&static_data_config);
    });

    static_data_task.join().unwrap();
    lesson_zip_task.join().unwrap();

    templates::process_content(config);
}
