export interface Pool {
  id: string;
  dex: string;
  tokenA: string;
  tokenB: string;
  reserveA: bigint;
  reserveB: bigint;
  feeBps: number;
  slot: number;
}
export interface Edge { from: string; to: string; pool: Pool; weight: number; }
export interface Path { tokens: string[]; pools: Pool[]; expectedProfit: number; }
