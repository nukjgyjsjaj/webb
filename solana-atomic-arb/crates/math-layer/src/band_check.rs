pub fn in_band(market_price: f64, pool_price: f64, gamma: f64) -> bool {
    gamma * market_price <= pool_price && pool_price <= market_price / gamma
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn noise_inside() {
        assert!(in_band(100.0, 100.0, 0.99));
        assert!(!in_band(100.0, 200.0, 0.99));
    }
}
