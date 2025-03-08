use std::env;
use std::fs;
use toml::Table;

fn main() {
    let args: Vec<String> = env::args().collect();

    let config: Table = fs::read_to_string("./config.toml")
        .expect(&format!("ERROR: Could not read config.toml in {}", std::env::current_dir().unwrap().display()))
        .parse::<Table>()
        .expect("ERROR: Could not parse config.toml");

    let port = config["Service"]["port_number"].as_integer().unwrap();
    let filename = config["Service"]["log_file"].as_str().unwrap().to_string();
    let rate_limit = config["Service"]["rate_limit"].as_integer().unwrap();

    for arg in args.iter() {
        match arg.as_str() {
            "-p" => println!("Port number: {}", port),
            "-l" => println!("Log file: {}", filename),
            "-r" => println!("Rate limit: {}", rate_limit),
            _ => println!("Unknown argument: {}", arg),
            
        }
    }
}
