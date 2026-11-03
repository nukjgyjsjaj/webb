pub fn pump_out(amount_in: u128, virtual_sol: u128, virtual_token: u128) -> u128 {
    // bonding curve: out = virtual_token * amount_in / (virtual_sol + amount_in)
    amount_in * virtual_token / (virtual_sol + amount_in)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn basic() {
        assert!(pump_out(1_000_000, 30_000_000_000, 1_000_000_000_000) > 0);
    }
}
