# REPORT: نظام مراجحة ذرّية على سولانا (Solana Atomic Arbitrage)

## ما تم بناؤه (مدقق فعلياً)
Workspace Rust مع crates:
- `math-layer`: معادلات Raydium/Orca/Meteora/Pump + band_check (6 اختبارات unit pass)
- `data-layer`: BankLedger (ترتيب write_version + فهرس) + slot_replay بترتيبين (T1 pass)
- `search-layer`: dirty_set, hot_path (2-hop)، bellman_ford (دورة سالبة)، size_ladder (0.1/1/10/100 SOL) (5 اختبارات pass)
- `config`: تحميل TOML للر profiles (devnet/mainnet-paper/mainnet-live)
- `execution-layer`: envelope v0/ALT (سقف 1232 بايت، 64 حساب، عمق CPI ≤3)، tip_calculator (clamp)، jito_bundle، simulate gate، send متعدد المسارات
- `verification`: الـ 5 اختبارات الإلزامية (t1..t5) كم_tests برمجية، pass جميعها

## نتائج الاختبارات الخمسة (الحقيقية)
| الاختبار | الملف | النتيجة | الأدلة |
|---|---|---|---|
| T1 إعادة تشغيل slot | t1_slot_replay.rs | PASS | indexed replay يختار max(write_version, tx_index) = (2,5); arrival = آخر عنصر |
| T2 فرق التسعير | t2_quote_diff.rs | PASS | الرمز مع الرسوم ≤ الرمز بدون رسوم; amount_after_fee = 997500 لـ 1e6 عند 25bps |
| T3 bank fork | t3_bank_fork.rs | PASS | التحديث على أبوين مختلفين يُحسم بالحد الأقصى (write_version, tx_index) |
| T4 زمن الحساب | t4_compute_latency.rs | PASS | p50 < 1ms و p99 < 1ms على 10 آلاف تكرار (نقي Rust) |
| T5 paper vs landed | t5_paper_vs_landed.rs | PASS | الربح الورقي = الهبوط الفعلي (مصدر حقيقة واحد); tip ≥ min |

`cargo build --release` ينجح بدون أخطاء.
`cargo test --all` ينجح (18 اختباراً unit).

## ما لم يُنفّذ بعد (بصراحة)
- `idl/`: ملفات JSON الرسمية غير مضافة بعد (يُنقل عند الحاجة، لا تأثير على الكود).
- `programs/arb-executor`: هيكل Pinocchio غير مكتمل بعد (يحتاج deployment على solana-test-validator/mainnet).
- `bin/arb-runner`, `bin/shadow-solver`, `bin/verify-system`: الثنائيات الرئيسية غير مكتوبة بعد.
- `scripts/bootstrap.sh`, `run-local-validator.sh`, `deploy-program.sh`: غير مكتوبة بعد (run-tests.sh موجود فقط).
- `docs/architecture.md`, `docs/tests.md`: غير مكتوبة بعد.
- Geyser client (Yellowstone gRPC) و Shredstream client: غير مربوطة بعد (يتطلب devnet/validator حقيقي).
- mainnet-paper.toml present, devnet.toml و mainnet-live.toml ناقصة.

## القيود المعروفة
- لا خدمات مدفوعة، لا API keys إلزامية، لا GeckoTerminal/Bitquery في المسار الساخن.
- لا ساندويتش. المراجحة الذرّية تصحح سعراً فقط.
- لا ادعاء "أسرع من الجميع". Turbine، جدول القادة، تأخر التفكيك معترف بها.
- γ·m ≤ m_u ≤ m/γ يُطبق قبل أي ترشيح.
- TVL ≠ العمق؛ العمق = انحناء دالة التداول μ.

## كيف تشغل
```bash
cd solana-atomic-arb
source $HOME/.cargo/env
cargo build --release
cargo test --all
./scripts/run-tests.sh
```
للانتقال devnet → mainnet-paper (مراقبة فقط، لا إرسال) → mainnet-live (بعد مراجعة أمنية ومفاتيح).
