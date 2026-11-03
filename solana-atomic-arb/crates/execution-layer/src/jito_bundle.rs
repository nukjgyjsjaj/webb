pub struct Bundle {
    pub transactions: Vec<Vec<u8>>,
    pub tip_lamports: u64,
}

impl Bundle {
    pub fn new() -> Self { Self { transactions: Vec::new(), tip_lamports: 0 } }
    pub fn add(&mut self, tx: Vec<u8>) { self.transactions.push(tx); }
    pub fn is_valid(&self) -> bool {
        !self.transactions.is_empty() && self.transactions.iter().all(|t| !t.is_empty())
    }
}
#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn bundle_valid() {
        let mut b = Bundle::new();
        assert!(!b.is_valid());
        b.add(vec![1,2,3]);
        assert!(b.is_valid());
    }
}
