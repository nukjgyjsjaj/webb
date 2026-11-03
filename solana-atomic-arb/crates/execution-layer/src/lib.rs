pub mod simulate;
pub mod send;
pub mod tip_calculator;
pub mod jito_bundle;

/// Deterministic transaction envelope (v0-style concept, ALT-ready).
#[derive(Debug, Clone)]
pub struct Envelope {
    pub bytes: Vec<u8>,
    pub tip_lamports: u64,
    pub size_limit: usize,
    pub max_accounts: usize,
    pub max_cpi_depth: usize,
}

impl Default for Envelope {
    fn default() -> Self {
        Self { bytes: Vec::new(), tip_lamports: 0, size_limit: 1232, max_accounts: 64, max_cpi_depth: 3 }
    }
}

pub fn validate_envelope(e: &Envelope) -> bool {
    e.bytes.len() <= e.size_limit && e.tip_lamports >= 0
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn envelope_default_ok() {
        let e = Envelope::default();
        assert!(validate_envelope(&e));
        assert_eq!(e.size_limit, 1232);
        assert_eq!(e.max_accounts, 64);
        assert_eq!(e.max_cpi_depth, 3);
    }
    #[test]
    fn size_reject_overlimit() {
        let mut e = Envelope::default();
        e.bytes.resize(1233, 0);
        assert!(!validate_envelope(&e));
    }
}
