pub fn amount_after_fee(amount_in: u128, num: u64, den: u64) -> u128 {
    amount_in - (amount_in * num as u128).div_ceil(den as u128)
}
pub fn raydium_out(amount_in: u128, in_reserve: u128, out_reserve: u128, num: u64, den: u64) -> u128 {
    let aaf = amount_after_fee(amount_in, num, den);
    aaf * out_reserve / (in_reserve + aaf)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn basic() {
        let out = raydium_out(1_000_000, 1_000_000_000, 1_000_000_000, 25, 10000);
        assert!(out > 0 && out < 1_000_000);
    }
    #[test]
    fn fee_applied() {
        let nofee = raydium_out(1_000_000, 1_000_000_000, 1_000_000_000, 0, 10000);
        let withfee = raydium_out(1_000_000, 1_000_000_000, 1_000_000_000, 25, 10000);
        assert!(withfee < nofee);
    }
}
