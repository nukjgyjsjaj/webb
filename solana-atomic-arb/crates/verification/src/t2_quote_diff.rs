use math_layer::{raydium_out, amount_after_fee};

pub fn t2_quote_diff_pass() {
    let base = raydium_out(1_000_000, 1_000_000_000, 1_000_000_000, 0, 10_000);
    let feef = raydium_out(1_000_000, 1_000_000_000, 1_000_000_000, 25, 10_000);
    assert!(feef <= base);
    let aaf = amount_after_fee(1_000_000, 25, 10_000);
    assert_eq!(aaf, 997_500);
}
