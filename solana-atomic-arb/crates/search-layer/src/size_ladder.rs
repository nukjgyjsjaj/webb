pub const SIZES: [u64; 4] = [10_000_000, 100_000_000, 1_000_000_000, 10_000_000_000];

pub fn iter_sizes() -> impl Iterator<Item = u64> { SIZES.iter().copied() }

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn ladder() {
        let v: Vec<u64> = iter_sizes().collect();
        assert_eq!(v.len(), 4);
        assert_eq!(v[0], 10_000_000);
    }
}
