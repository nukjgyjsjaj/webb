import { Connection } from "@solana/web3.js";
import { CONFIG } from "./config.js";
import { buildEdges, findArbPaths } from "./graph.js";
import { isProfitable } from "./safety.js";
import type { Pool } from "./types.js";

async function main() {
  const conn = new Connection(CONFIG.rpcUrl, "confirmed");
  console.log("solana-arb boot", CONFIG.rpcUrl);
  // Placeholder pipeline: real impl subscribes to DEX accounts via Helius WS,
  // decodes IDLs into Pool[], builds graph, finds paths, simulates, sends via TPU.
  const pools: Pool[] = [];
  const edges = buildEdges(pools);
  const paths = findArbPaths(edges, "SOL", 4);
  const viable = paths.filter((p) => isProfitable(p, CONFIG.flashFeeBps));
  console.log("paths", paths.length, "viable", viable.length);
  try { await conn.getSlot(); console.log("rpc ok"); } catch (e) { console.warn("rpc unavailable (set RPC_URL):", (e as Error).message); }
}

main().catch((e) => { console.error(e); process.exit(1); });
