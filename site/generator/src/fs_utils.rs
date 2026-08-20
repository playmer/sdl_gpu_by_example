use std::path::{Path, PathBuf};

use natural_sort_rs::NaturalSort;
use walkdir::WalkDir;

fn get_folders_or_paths(asset_dir: &Path, want_dirs: bool) -> anyhow::Result<Vec<PathBuf>> {
    let mut paths: Vec<PathBuf> = Vec::new();

    let walkdir = WalkDir::new(asset_dir);
    let it = &mut walkdir.into_iter().filter_map(|e| e.ok());

    for entry in it {
        let entry_path_buf = entry.into_path();
        let entry_path = entry_path_buf.strip_prefix(asset_dir)?;

        if entry_path.as_os_str().is_empty() {
            continue;
        }

        if want_dirs {
            if 1 != entry_path.components().count() {
                continue;
            }

            if entry_path_buf.is_dir() {
                paths.push(entry_path.to_path_buf());
            }
        } else if entry_path_buf.is_file() {
            paths.push(entry_path.to_path_buf());
        }
    }

    paths.natural_sort_by_cached_key::<[u8], _, _>(|x| x.as_os_str().as_encoded_bytes().to_owned());

    Ok(paths)
}

pub fn get_folders(asset_dir: &Path) -> anyhow::Result<Vec<PathBuf>> {
    get_folders_or_paths(asset_dir, true)
}

pub fn get_files(asset_dir: &Path) -> anyhow::Result<Vec<PathBuf>> {
    get_folders_or_paths(asset_dir, false)
}
