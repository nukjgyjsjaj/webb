import { Connection, Keypair, Transaction, VersionedTransaction } from "@solana/web3.js";

export async function currentLeaderEndpoint(conn: Connection): Promise<string | null> {
  try {
    const epoch = await conn.getEpochInfo();
    const leaders = await conn.getSlotLeaders(epoch.absoluteSlot, 1);
    return leaders.length ? `tpu://${leaders[0]}` : null;
  } catch {
    return null;
  }
}

export async function sendToTpu(
  _conn: Connection,
  _tx: VersionedTransaction | Transaction,
  _keypair: Keypair
): Promise<string> {
  // Direct TPU QUIC submission would use @solana/web3.js TpuClient (experimental).
  // Fallback: RPC sendRawTransaction (best-effort) until TPU client wired.
  throw new Error("TPU client not wired; implement QUIC submit to leader endpoint");
}
