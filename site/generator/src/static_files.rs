use std::fs;

use anyhow::Context;

use crate::config::BuildConfig;
use crate::fs_utils;

pub fn write_static_data(config: &BuildConfig) -> anyhow::Result<()> {
    for file_source in fs_utils::get_files(&config.static_data_dir)? {
        let file_destination = config.output_dir.join(&file_source);

        let parent = file_destination.parent().with_context(|| {
            format!(
                "{} + {} has no parent directory. Is the output directory broken?",
                config.output_dir.display(),
                file_source.display()
            )
        })?;

        fs::create_dir_all(parent)?;
        fs::copy(config.static_data_dir.join(&file_source), &file_destination)?;
    }

    Ok(())
}
