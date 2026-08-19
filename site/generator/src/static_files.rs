use std::fs;
use std::path::Path;

use crate::config;
use crate::fs_utils;

pub fn write_static_data(output_dir: &Path) {
    let static_data_dir = Path::new(config::STATIC_DATA_DIR);

    for file_source in fs_utils::get_files(static_data_dir) {
        let file_destination = output_dir.join(&file_source);

        fs::create_dir_all(file_destination.parent().unwrap()).unwrap();
        fs::copy(static_data_dir.join(&file_source), &file_destination).unwrap();
    }
}