import { CONFIG } from "./config.js";
import { Path, Pool } from "./types.js";

export function isProfitable(path: Path, flashFeeBps: number): boolean {
  const gross = path.expectedProfit;
  const fees = (flashFeeBps + path.pools.reduce((a, p) => a + p.feeBps, 0)) / 10000;
  return gross - fees > CONFIG.minProfitLamports / 1e9;
}

export function slippageOk(pool: Pool, amountIn: number): boolean {
  const impact = amountIn / Number(pool.reserveA);
  return impact * 10000 < CONFIG.maxSlippageBps;
}

export class KillSwitch {
  private tripped = false;
  trip(reason: string) { this.tripped = true; console.error("KILLSWITCH", reason); }
  isTripped() { return this.tripped; }
}
