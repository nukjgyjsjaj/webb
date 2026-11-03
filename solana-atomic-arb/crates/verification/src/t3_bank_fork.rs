use data_layer::{BankLedger, AccountState, ReplayOrder};

pub fn t3_bank_fork_pass() {
    let mut l = BankLedger::new();
    let key = b"fork-key".to_vec();
    l.apply(key.clone(), AccountState { data: vec![1], write_version: 1, tx_index: 1, bank_id: 2 });
    l.apply(key.clone(), AccountState { data: vec![2], write_version: 2, tx_index: 0, bank_id: 3 });
    let st = l.replay_slot(&key, ReplayOrder::Indexed).unwrap();
    assert_eq!(st.data, vec![2]);
}
