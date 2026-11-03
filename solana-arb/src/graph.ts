import { Pool, Edge, Path } from "./types.js";

export function buildEdges(pools: Pool[]): Edge[] {
  const edges: Edge[] = [];
  for (const p of pools) {
    const priceAB = Number(p.reserveB) / Number(p.reserveA);
    const priceBA = Number(p.reserveA) / Number(p.reserveB);
    const f = 1 - p.feeBps / 10000;
    edges.push({ from: p.tokenA, to: p.tokenB, pool: p, weight: priceAB * f });
    edges.push({ from: p.tokenB, to: p.tokenA, pool: p, weight: priceBA * f });
  }
  return edges;
}

export function findArbPaths(edges: Edge[], startToken: string, maxHops: number): Path[] {
  const nodes = new Set<string>();
  for (const e of edges) { nodes.add(e.from); nodes.add(e.to); }
  const dist = new Map<string, number>();
  const prev = new Map<string, { edge: Edge; node: string } | null>();
  for (const n of nodes) { dist.set(n, n === startToken ? 1 : 0); prev.set(n, null); }
  for (let i = 0; i < maxHops; i++) {
    let updated = false;
    for (const e of edges) {
      const d = (dist.get(e.from) ?? 0) * e.weight;
      if (d > (dist.get(e.to) ?? 0)) {
        dist.set(e.to, d);
        prev.set(e.to, { edge: e, node: e.from });
        updated = true;
      }
    }
    if (!updated) break;
  }
  const paths: Path[] = [];
  for (const e of edges) {
    const d = (dist.get(e.from) ?? 0) * e.weight;
    if (d > (dist.get(e.to) ?? 0) * 1.0001) {
      const tokens: string[] = [];
      const pools: Pool[] = [];
      let cur: string | null = e.to;
      const seen = new Set<string>();
      while (cur && !seen.has(cur)) {
        seen.add(cur);
        tokens.unshift(cur);
        const p = prev.get(cur);
        if (p) { pools.unshift(p.edge.pool); cur = p.node; } else break;
      }
      tokens.unshift(startToken);
      paths.push({ tokens, pools, expectedProfit: d - 1 });
    }
  }
  return paths;
}
