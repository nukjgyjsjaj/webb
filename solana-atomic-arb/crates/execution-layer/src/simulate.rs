pub struct SimResult { pub ok: bool, pub units_consumed: u64 }

pub fn simulate(bytes: &[u8]) -> SimResult {
    SimResult { ok: !bytes.is_empty(), units_consumed: bytes.len() as u64 }
}
pub fn gate_send(sim: &SimResult) -> bool { sim.ok }
#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn gate() {
        assert!(gate_send(&simulate(&[1,2])));
        assert!(!gate_send(&simulate(&[])));
    }
}
