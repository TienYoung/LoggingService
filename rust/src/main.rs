use std::env;
use std::fs;
use toml::Table;

fn main() {
    let args: Vec<String> = env::args().collect();
    let mut port = 50000;
    let mut filename = String::from("log.lnav");
    let mut rate_limit = 100;

    let config: Table = fs::read_to_string("config.toml")
        .expect("ERROR: Could not read config.toml")
        .parse::<Table>()
        .expect("ERROR: Could not parse config.toml");

    port = config["Service"]["port_number"].as_integer().unwrap();
    filename = config["Service"]["log_file"].as_str().unwrap().to_string();
    rate_limit = config["Service"]["rate_limit"].as_integer().unwrap();

    println!("Port: {}", port);
    println!("Filename: {}", filename);
    println!("Rate Limit: {}", rate_limit);
}
