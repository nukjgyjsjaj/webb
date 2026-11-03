pub fn tip_calculator(profit_lamports: u64, ratio_bps: u64, min_tip: u64, max_tip: u64) -> u64 {
    let raw = profit_lamports * ratio_bps / 10_000;
    raw.clamp(min_tip, max_tip)
}
#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn clamped() {
        assert_eq!(tip_calculator(1_000_000, 1000, 10_000, 1_000_000), 100_000);
        assert_eq!(tip_calculator(1_000_000, 1000, 500_000, 1_000_000), 500_000);
        assert_eq!(tip_calculator(1_000_000, 1000, 10_000, 10_000), 10_000);
    }
}
