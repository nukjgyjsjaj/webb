use crate::dirty_set::DirtySet;

pub struct Edge { pub from: String, pub to: String, pub rate: f64 }

pub fn enumerate_2hop(edges: &[Edge], dirty: &DirtySet) -> Vec<(String, String, f64)> {
    let mut out = Vec::new();
    for e in edges {
        if !dirty.contains(&e.from) && !dirty.contains(&e.to) { continue; }
        for e2 in edges {
            if e2.from == e.to {
                let rate = e.rate * e2.rate;
                out.push((e.from.clone(), e2.to.clone(), rate));
            }
        }
    }
    out
}

#[cfg(test)]
mod tests {
    use super::*;
    fn setup() -> (Vec<Edge>, DirtySet) {
        let edges = vec![
            Edge { from: "SOL".into(), to: "USDC".into(), rate: 100.0 },
            Edge { from: "USDC".into(), to: "WSOL".into(), rate: 0.01 },
            Edge { from: "SOL".into(), to: "WSOL".into(), rate: 1.0 },
        ];
        let mut d = DirtySet::new();
        d.mark("USDC");
        (edges, d)
    }
    #[test]
    fn finds_dirty_two_hop() {
        let (edges, d) = setup();
        let r = enumerate_2hop(&edges, &d);
        assert!(r.iter().any(|(a, b, _)| a == "SOL" && b == "WSOL"));
    }
}
