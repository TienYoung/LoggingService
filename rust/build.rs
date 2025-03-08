use std::env;
use std::fs;
use std::path::PathBuf;

fn main() {
    let manifest_dir_string = env::var("CARGO_MANIFEST_DIR").unwrap();
    let build_type = env::var("PROFILE").unwrap();
    let dest_path = PathBuf::from(manifest_dir_string)
        .join("Target")
        .join(build_type)
        .join("config.toml");
    fs::copy("../config.toml", &dest_path).expect("Failed to copy config.toml");
}