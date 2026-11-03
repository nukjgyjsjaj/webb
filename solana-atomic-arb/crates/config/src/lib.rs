use std::fs;
use serde::{Deserialize, Serialize};

#[derive(Debug, Deserialize, Serialize)]
pub struct AppConfig {
    pub rpc_url: String,
    pub ws_url: String,
    pub keypair_path: String,
    pub gamma: f64,
    pub min_tip_lamports: u64,
}

impl AppConfig {
    pub fn load(path: &str) -> Result<Self, Box<dyn std::error::Error>> {
        let s = fs::read_to_string(path)?;
        Ok(toml::from_str(&s)?)
    }
}
