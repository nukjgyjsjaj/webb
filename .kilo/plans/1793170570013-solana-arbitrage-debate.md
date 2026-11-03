# خطة: إنشاء AGENTS.md للمستودع

## السياق
- مستودع صغير: مشروع Android (`amnesia-android`) + سكربت سولانا (`solana-arb/`, TypeScript) + ملفات خطة (`.kilo/`).
- لا يوجد `AGENTS.md` حالياً.
- يجب أن تكون الإجابات بالعربية دائماً.

## المهام (للوكيل (code))
1. إنشاء `/workspace/app/AGENTS.md` (عربي، مختصر) يحتوي:
   - أوامر البناء/التشغيل لـ `solana-arb`: `npm install`, `npm run build`, `npm run start`, `npm run dev`.
   - متغيرات البيئة: `RPC_URL`, `WS_URL`, `KEYPAIR`, `FLASH`, `FLASH_FEE_BPS`, `MIN_PROFIT`, `MAX_SLIPPAGE_BPS`.
   - ملاحظة: `tpu.ts` غير مربوط (throws) — cần wiring.
   - ملاحظة: `node_modules/` و `dist/` غير متجاهلة (أضف `.gitignore`).
   - Android: Gradle project under `amnesia-android/`.
2. إضافة `.gitignore` (node_modules, dist).
3. (اختياري) تحقق `npm run build`.

## المخاطر
- لا تعدل ملفات source خارج النطاق.
- احفظ الإعدادات بالعربية (تفضيل المستخدم).

## التحقق
- `npm run build` ينجح.
- `git status` نظيف (باستثناء untracked المقصود).
