use std::net;
use std::process;
use std::io::prelude::*;
use serde::Serialize;
use clap::{CommandFactory, Parser};
use chrono::Utc;
use gethostname::gethostname;
use uuid::Uuid;

#[derive(Parser)]
struct Args {
    #[arg(long)]
    host: String,
    #[arg(long)]
    port: u16,
}

#[derive(Serialize)]
struct Log {
    priority: i32,
    timestamp: String,
    hostname: String,
    application: String,
    pid: u32,
    msgid: String,
    message: String,
}

fn main() {
    let args = Args::parse();

    let host = args.host;
    let port= args.port;

    let mut stream = net::TcpStream::connect((host, port)).unwrap();
    
    let log = Log {
        priority: 1,
        timestamp: Utc::now().to_rfc3339().to_string(),
        hostname: gethostname().to_string_lossy().to_string(),
        application: Args::command().get_name().to_string(),
        pid: process::id(),
        msgid: Uuid::new_v4().to_string(),
        message: "Hello, this is Rust!".to_string(),
    };

    stream.write(serde_json::to_string(&log).unwrap().as_bytes()).unwrap();
}
