export const CONFIG = {
  rpcUrl: process.env.RPC_URL ?? "https://mainnet.helius-rpc.com/?api-key=KEY",
  wsUrl: process.env.WS_URL ?? "wss://mainnet.helius-rpc.com/?api-key=KEY",
  keypairPath: process.env.KEYPAIR ?? "./keypair.json",
  flashLoanProvider: process.env.FLASH ?? "kamino",
  flashFeeBps: Number(process.env.FLASH_FEE_BPS ?? 5),
  minProfitLamports: Number(process.env.MIN_PROFIT ?? 10000),
  maxSlippageBps: Number(process.env.MAX_SLIPPAGE_BPS ?? 50),
  cuLimit: 1_300_000,
  staleSlotTtl: 2,
  topDexCount: 6,
};
