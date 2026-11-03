pub mod t1_slot_replay;
pub mod t2_quote_diff;
mod t3_bank_fork;
mod t4_compute_latency;
mod t5_paper_vs_landed;
pub use t1_slot_replay::*;
pub use t2_quote_diff::*;
pub use t3_bank_fork::*;
pub use t4_compute_latency::*;
pub use t5_paper_vs_landed::*;

#[cfg(test)]
mod all_tests {
    use super::*;
    #[test]
    fn suite_passes() {
        t1_slot_replay_pass();
        t2_quote_diff_pass();
        t3_bank_fork_pass();
        t4_compute_latency_pass();
        t5_paper_vs_landed_pass();
    }
}
