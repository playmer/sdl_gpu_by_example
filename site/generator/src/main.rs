use std::env;

use warp::Filter;

pub mod build;
pub mod config;
pub mod content;
pub mod diff;
pub mod lessons;
pub mod fs_utils;
pub mod markdown;
pub mod static_files;
pub mod templates;


#[tokio::main]
async fn main() {

    build::build_site();

    let args: Vec<String> = env::args().collect();

    if !args.contains(&"--no-serve".to_owned())
    {
        println!("Link to site: http://127.0.0.1:4040/sdl_gpu_by_example/");

        let site = warp::path("sdl_gpu_by_example")
            .and(warp::fs::dir(config::OUTPUT_DIR));

        warp::serve(site)
            .run(([127, 0, 0, 1], 4040))
            .await;
    }
}
