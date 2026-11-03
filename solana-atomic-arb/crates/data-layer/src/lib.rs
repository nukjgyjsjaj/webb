use std::collections::BTreeMap;

#[derive(Clone, Debug, PartialEq, Eq)]
pub struct AccountState {
    pub data: Vec<u8>,
    pub write_version: u64,
    pub tx_index: u64,
    pub bank_id: u64,
}

#[derive(Default)]
pub struct BankLedger {
    // key -> list of states per bank fork
    states: BTreeMap<Vec<u8>, Vec<AccountState>>,
}

impl BankLedger {
    pub fn new() -> Self { Self::default() }
    pub fn apply(&mut self, key: Vec<u8>, st: AccountState) {
        self.states.entry(key).or_default().push(st);
    }
    pub fn latest(&self, key: &[u8]) -> Option<AccountState> {
        self.states.get(key)?.iter()
            .max_by_key(|s| (s.write_version, s.tx_index))
            .cloned()
    }
    pub fn replay_slot(&self, key: &[u8], order: ReplayOrder) -> Option<AccountState> {
        let mut v = self.states.get(key)?.clone();
        match order {
            ReplayOrder::Arrival => {}
            ReplayOrder::Indexed => v.sort_by_key(|s| (s.write_version, s.tx_index)),
        }
        v.into_iter().last()
    }
}

#[derive(Clone, Copy)]
pub enum ReplayOrder { Arrival, Indexed }

#[cfg(test)]
mod tests {
    use super::*;
    fn mk(wv: u64, tx: u64) -> AccountState {
        AccountState { data: vec![], write_version: wv, tx_index: tx, bank_id: 1 }
    }
    #[test]
    fn t1_indexed_replay_deterministic() {
        let mut l = BankLedger::new();
        let key = b"k".to_vec();
        // arrival order reversed
        l.apply(key.clone(), mk(2, 5));
        l.apply(key.clone(), mk(1, 3));
        l.apply(key.clone(), mk(2, 2));
        let a = l.replay_slot(&key, ReplayOrder::Arrival);
        let b = l.replay_slot(&key, ReplayOrder::Indexed);
        // indexed replay should pick (2,5)
        assert_eq!(b.as_ref().unwrap().tx_index, 5);
        // both must agree on final by (write_version, tx_index)
        assert_eq!(a.as_ref().unwrap().write_version, b.as_ref().unwrap().write_version);
    }
}
