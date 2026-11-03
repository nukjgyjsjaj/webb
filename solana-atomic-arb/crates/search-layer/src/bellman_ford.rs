pub struct BFEdge { pub from: usize, pub to: usize, pub weight: f64 }

pub fn detect_negative_cycle(edges: &[BFEdge], n: usize) -> bool {
    let mut dist = vec![0.0f64; n];
    for _ in 0..n {
        let mut updated = false;
        for e in edges {
            if dist[e.from] + e.weight < dist[e.to] - 1e-12 {
                dist[e.to] = dist[e.from] + e.weight;
                updated = true;
            }
        }
        if !updated { return false; }
    }
    for e in edges {
        if dist[e.from] + e.weight < dist[e.to] - 1e-12 { return true; }
    }
    false
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn detects_cycle() {
        let edges = vec![
            BFEdge { from: 0, to: 1, weight: -1.0 },
            BFEdge { from: 1, to: 0, weight: -1.0 },
        ];
        assert!(detect_negative_cycle(&edges, 2));
    }
    #[test]
    fn no_cycle() {
        let edges = vec![BFEdge { from: 0, to: 1, weight: 1.0 }];
        assert!(!detect_negative_cycle(&edges, 2));
    }
}
