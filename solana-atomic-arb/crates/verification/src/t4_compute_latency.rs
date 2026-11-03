use std::time::Instant;
use math_layer::band_check::in_band;
use execution_layer::simulate::gate_send;
use execution_layer::simulate::simulate;

pub fn t4_compute_latency_pass() {
    let mut times: Vec<u64> = Vec::new();
    for _ in 0..10_000 {
        let start = Instant::now();
        let _ = in_band(100.0, 100.0, 0.99);
        let _ = gate_send(&simulate(&[1,2,3]));
        times.push(start.elapsed().as_nanos() as u64);
    }
    times.sort_unstable();
    let p50 = times[times.len()/2] as f64 / 1_000_000.0;
    let p99 = times[(times.len()*99/100).min(times.len()-1)] as f64 / 1_000_000.0;
    assert!(p50 < 1.0, "p50 = {} ms", p50);
    assert!(p99 < 1.0, "p99 = {} ms", p99);
}
