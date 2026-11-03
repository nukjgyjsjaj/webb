pub fn orca_out(amount_in: u128, sqrt_price_x64: u128, liquidity: u128) -> u128 {
    // simplified: delta a = L * (sqrt_p_hi - sqrt_p_lo)/(sqrt_p_hi*sqrt_p_lo)
    if liquidity == 0 { return 0; }
    amount_in * liquidity / (sqrt_price_x64.max(1))
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn nonzero() {
        assert!(orca_out(1_000_000, 1_000_000, 1_000_000_000) > 0);
    }
}
