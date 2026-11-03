use math_layer::raydium_out;
use execution_layer::tip_calculator::tip_calculator;

pub fn t5_paper_vs_landed_pass() {
    let paper = raydium_out(10_000_000, 100_000_000, 100_000_000, 30, 10_000);
    assert!(paper > 0);
    let tip = tip_calculator(10_000_000, 1000, 10_000, 1_000_000);
    assert!(tip >= 10_000);
    let landed = raydium_out(10_000_000, 100_000_000, 100_000_000, 30, 10_000);
    assert_eq!(paper, landed);
}
