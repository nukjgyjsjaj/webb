# AGENTS.md

## مشروع
- Android app under `amnesia-android/` (Gradle).
- Solana arbitrage skeleton under `solana-arb/` (TypeScript, Node).

## أوامر (solana-arb)
- تثبيت: `npm install`
- بناء: `npm run build`
- تشغيل: `npm run start`
- تطوير: `npm run dev`

## متغيرات البيئة
- `RPC_URL`, `WS_URL`, `KEYPAIR`, `FLASH`, `FLASH_FEE_BPS`, `MIN_PROFIT`, `MAX_SLIPPAGE_BPS`

## ملاحظات
- `src/tpu.ts` غير مربوط (throws) — يحتاج wiring QUIC/RPC.
- `node_modules/` و `dist/` متجاهلة عبر `.gitignore`.
- الخطة (debate) في `.kilo/plans/`.

## تفضيل
- الردود بالعربية دائماً.
