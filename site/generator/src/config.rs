use std::path::{Path, PathBuf};

use anyhow::Context;

pub static NO_ESCAPE: &str = "<!-- NO_ESCAPE -->";

const CONFIG_FILE: &str = "generator.toml";

#[derive(Clone, Debug)]
pub struct BuildConfig {
    pub code_dir: PathBuf,
    pub code_source_dir: PathBuf,
    pub code_asset_dir: PathBuf,
    pub code_cmake_dir: PathBuf,
    pub content_dir: PathBuf,
    pub inserts_dir: PathBuf,
    pub static_data_dir: PathBuf,
    pub template_dir: PathBuf,
    pub output_dir: PathBuf,
}

impl BuildConfig {
    pub fn discover() -> anyhow::Result<BuildConfig> {
        let current_dir = std::env::current_dir()?;
        let project_root = find_project_root(&current_dir)?;

        Ok(BuildConfig {
            code_dir: project_root.join("code"),
            code_source_dir: project_root.join("code/source"),
            code_asset_dir: project_root.join("code/Assets"),
            code_cmake_dir: project_root.join("code/CMake"),
            content_dir: project_root.join("site/content"),
            inserts_dir: project_root.join("site/inserts"),
            static_data_dir: project_root.join("site/static_data"),
            template_dir: project_root.join("site/template"),
            output_dir: project_root.join("site/generator/output"),
        })
    }
}

fn find_project_root(start: &Path) -> anyhow::Result<PathBuf> {
    start
        .ancestors()
        .find(|directory| directory.join(CONFIG_FILE).is_file())
        .map(Path::to_path_buf)
        .with_context(|| {
            format!(
                "could not find {CONFIG_FILE} in {} or any parent directory",
                start.display()
            )
        })
}
