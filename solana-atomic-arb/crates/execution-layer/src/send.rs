pub enum Path { Jito, TpuDirect, FreeTier }

pub fn send(bytes: &[u8], path: Path) -> bool {
    !bytes.is_empty() && matches!(path, Path::Jito | Path::TpuDirect | Path::FreeTier)
}
#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn sends() {
        assert!(send(&[1], Path::Jito));
        assert!(!send(&[], Path::TpuDirect));
    }
}
