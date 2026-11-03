# مداولة 2 — نظام مراجحة سولانا (Flash Loan + Direct TPU)

## استدعاء الخبير

**المايسترو (صامت):** *إيماءة.*

**الخبير المتخصص — د. يوكي، مهندس MEV على سولانا (Jito/TPU):** "أنا هنا. الفكرة: بحث MEV تنفيذي، not bot. سأحلل الجذور والكوارث من الدرجة الثانية."

---

## طرح المهندس — المسودة

**المهندس:** "1) قرض فوري 'مجاني بدون رسوم'. 2) Helius RPC + 24+ DEX IDLs (أسعار/رسوم/عمق). 3) أفضل 경로 (2/3/4+ hops) حسب الربح-الرسوم-العلاقة. 4) ATA لتقليل <1000 بايت. 5) إرسال مباشر لطابور المدقق (TPU) بدون RPC، بدون رسوم أولوية/رشوة."

---

## المعركة

**محامي الشيطان:** "أوقف. 'مجاني بدون رسوم' كذبة: كل مزود (Kamino/Marginfi/Solend/Mango) يأخذ 0–0.1% أو رسوم بروتوكول. عيب قاتل #1. أيضاً flash loan ذري في معاملة واحدة؛ 4 hops + flash = تجاوز compute units (~1.4M). عيب قاتل #2."

**الزبون الغبي:** "Зачем 24 DEX? Если один даёт лучшую цену, используй его. ATA creation costs rent и УВЕЛИЧИВАЕТ размер. Solana max tx = 1232 bytes. 4-hop + flash + compute budget > 1232. Математика фантазия. Fatal #3."

**الخبير:** "جذور: (أ) flash loan = atomic borrow, رسوم إلزامية. (ب) TPU: leader rotates every 4 slots (~1.6s); must target current leader QUIC endpoint. (ج) Size: استخدم Address Lookup Tables (ALT), not ATA — ATA solves existence not size. (د) Second-order: leader may reorder; competitors with Jito tips outbid; 'no priority fee' = lowest inclusion. (هـ) 24 IDLs = decoding overhead + stale state; need WS subs. (و) Market depth: price alone useless; need reserves+slippage. (ز) Triangular+: Bellman-Ford on graph, weight=(1-fee)*price."

**المهندس (تعديل):** "1) اختر أرخص مزود، احتسب الرسوم. 2) ALT pre-created. 3) TPU: leader schedule, QUIC, fallback RPC. 4) Helius WS sharded. 5) depth/slippage. 6) Bellman-Ford per block."

**محامي الشيطان:** "'بدون رسوم أولوية' سخيفة: base fee 5000 lamports إلزامي؛ بدون priority = drop при congestion. TPU direct = no backpressure; buffer full → dropped silently. Fatal: no observability. Also compute heavy IDL decode."

**الزبون الغبي:** "'Без посредников' но TPU это интерфейс валидатора. MEV searchers уже делают это с Jito; ты будешь последним. No bribe = no inclusion. Fatal: competing paid with worse tools."

**الخبير:** "Second-order: (1) alpha decays if succeed. (2) some validators drop non-Jito. (3) price movement between detection/execution = slippage; stale-price guard. (4) CU: versioned tx + ALT + compute budget. (5) pre-create ATAs. (6) Helius WS limits; shard. (7) negative cycle per block. (8) sandwich risk; TPU reduces not eliminates."

**المهندس (تعديل 2):** "pre-create ATAs; ALT; compute budget; slippage guards; leader-aware TPU retry; WS sharding; Bellman-Ford; include all fees in profit; fallback Jito bundle (optional tip). Drop 'zero fee' claim."

**محامي الشيطان:** "'Fallback Jito' contradicts 'no bribe'. Choose: pure TPU (unreliable) or Jito (tips). Also need simulateTransaction gate (latency tradeoff)."

**الزبون الغبي:** "'More than 24 DEX' scope creep. Start top 5-6 by volume. Fatal: scope creep."

**الخبير:** "Final principles: (1) Profit = gross diff - flash fee - swap fees - base fee - slippage - MEV risk. (2) Execution: leader-aware TPU QUIC, versioned tx, ALT, ATAs, compute budget. (3) Data: Helius WS, IDL decode, reserve-based depth. (4) Path: negative-cycle, top-K, simulate, pick best. (5) Safety: guards, CU cap, TTL, kill switch. (6) Honest: no 'free', no 'no fees', TPU best-effort."

---

## مرحلة المايسترو — الإجابة النهائية

**المايسترو:**

### الحل النهائي (جاهز للتنفيذ)

1. **Flash Loan:** أرخص مزود (Kamino/Marginfi)؛ احتسب الرسوم. ذري.
2. **Data:** Helius WS sharded لـ 5–8 DEX (top by volume)، IDL decode، reserves+fees+depth، TTL ≤ slot.
3. **Path:** graph (tokens nodes, pools edges, weight=(1-fee)*price). Bellman-Ford/negative-cycle per block. top-K by net profit (2/3/4+ hops).
4. **Tx:** versioned + ALT pre-created + pre-created ATAs + compute budget + slippage guards + CU cap (~1.4M).
5. **Exec:** leader schedule → QUIC TPU → direct send, retry next leader. simulateTransaction gate when latency allows. Fallback: Jito bundle with tip only if TPU fails.
6. **Safety:** kill switch, stale-price guard, max slippage, dry-run mode, logging.
7. **Honest constraints:** base fee 5000 lamports إلزامي؛ "مجاني" غير موجود؛ TPU غير مضمون؛ alpha decays.

**Risks:** alpha decay, validator policies, sandwich, CU limits, ALT rent.  
**Validation:** dry-run devnet/mainnet-fork; inclusion rate; CU profiling; slippage backtests.

---

**End.**
