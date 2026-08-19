use std::fs;

use crate::config::BuildConfig;
use crate::fs_utils;

pub fn write_static_data(config: &BuildConfig) {
    for file_source in fs_utils::get_files(&config.static_data_dir) {
        let file_destination = config.output_dir.join(&file_source);

        fs::create_dir_all(file_destination.parent().unwrap()).unwrap();
        fs::copy(config.static_data_dir.join(&file_source), &file_destination).unwrap();
    }
}