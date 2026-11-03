# Architecture

خمسة مكونات:
1. العين (data-layer): Yellowstone gRPC + Shredstream + BankLedger (trusted local state) + slot_replay.
2. الدماغ (math-layer): معادلات Raydium/Orca/Meteora/Pump من الكود المصدري (لا IDL)، band_check γ·m ≤ m_u ≤ m/γ.
3. البحث (search-layer): dirty-set 2-hop/3-hop + Bellman-Ford + shadow MICP (خيط ظل) + size ladder.
4. اليد (execution-layer): envelope v0 + ALT (1232 بايت، 64 حساب، CPI ≤3)، tip_calculator، simulate إلزامي قبل الإرسال، Jito bundle.
5. التحقق (verification): 5 اختبارات إلزامية T1..T5.

تدفق: البيانات →eket quotes → dirty set → البحث عن دورة سالبة → tip → simulate gate → إرسال عبر Jito/TPU مباشر.
