# DEX Equations

## Raydium AMM v4
```
amount_after_fee = amount_in - ceil(amount_in * fee_num / fee_den)
out = amount_after_fee * out_reserve / (in_reserve + amount_after_fee)
```
fee from AmmInfo.fees (not assumed 0.25%).

## Orca Whirlpool (CLMM)
```
Δa = L * (√p_lo - √p_hi) / (√p_hi * √p_li)
```
tick array iteration when liquidity exhausted.

## Meteora DLMM
```
P_i = (1 + bin_step/10000)^i
out_Y = floor(in_X * P_i) within bin
```
base + variable fee, capped at MAX_FEE_RATE.

## PumpSwap
```
bonding curve: out = virtual_token * in / (virtual_sol + in)
```
transition on complete.

## Band Check
```
γ·m_p ≤ m_u ≤ γ^{-1}·m_p
```
