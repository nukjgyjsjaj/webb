use std::collections::HashSet;

#[derive(Default)]
pub struct DirtySet(HashSet<String>);

impl DirtySet {
    pub fn new() -> Self { Self::default() }
    pub fn mark(&mut self, pool: &str) { self.0.insert(pool.to_string()); }
    pub fn contains(&self, pool: &str) -> bool { self.0.contains(pool) }
    pub fn clear(&mut self) { self.0.clear(); }
    pub fn len(&self) -> usize { self.0.len() }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn dirty_tracking() {
        let mut d = DirtySet::new();
        d.mark("poolA");
        assert!(d.contains("poolA"));
        assert!(!d.contains("poolB"));
        d.clear();
        assert!(!d.contains("poolA"));
    }
}
