pub fn meteora_out(amount_in: u128, bin_step_bps: u64, bin_id: i32) -> u128 {
    // P_i = (1 + bin_step/10000)^i ; out = floor(in * P)
    let mut price = 1.0f64;
    let step = bin_step_bps as f64 / 10000.0;
    for _ in 0..bin_id.abs() as usize {
        price *= 1.0 + step;
    }
    (amount_in as f64 * price).floor() as u128
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn basic() {
        assert!(meteora_out(1_000_000, 10, 5) >= 1_000_000);
    }
}
