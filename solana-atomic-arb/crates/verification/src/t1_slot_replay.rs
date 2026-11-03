use data_layer::{BankLedger, AccountState, ReplayOrder};

pub fn t1_slot_replay_pass() {
    let mut l = BankLedger::new();
    let key = b"pool-A".to_vec();
    for (wv, tx) in [(1u64,3),(2u64,5),(1u64,2)] {
        l.apply(key.clone(), AccountState { data: vec![wv as u8], write_version: wv, tx_index: tx, bank_id: 1 });
    }
    // arrival order returns last pushed: (1,2)
    let arrival = l.replay_slot(&key, ReplayOrder::Arrival).unwrap();
    assert_eq!(arrival.tx_index, 2);
    // indexed returns max by (write_version, tx_index): (2,5)
    let indexed = l.replay_slot(&key, ReplayOrder::Indexed).unwrap();
    assert_eq!(indexed.tx_index, 5);
    assert_eq!(indexed.write_version, 2);
}
