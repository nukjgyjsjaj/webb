## Known limitations (لا تُخفي)

- Yellowstone gRPC / Shredstream: إن لم تتوفر، تم استخدام `SubscribeDeshred` المفتوح المصدر (Triton) في `data-layer/geyser_client.rs`. التجربة الحقيقية تتطلب mainnet-beta validator مع Geyser.
- Jito block-engine العام: اختبر فقط في devnet/test-validator. الإنتاج يحتاج key + tip calculator.
- MICP (shadow_micp.rs): التشغيل في المسار الساخن ممنوع؛ يُشغل في خيط ظل (background thread) فقط.
- GeckoTerminal/Bitquery: excluded من المسار الساخن، للتدقيق اللاحق فقط.
- speed claims: النظام يعترف بحدوده الفيزيائية (Turbine، leader schedule).
- Pinocchio: البرنامج على السلسلة (`arb-executor`) غير مُدرّب بعد - يحتاج deployment.
- 5 اختبارات: `t1_slot_replay.rs` و `t2_quote_diff.rs` pass. `t3..t5` قيد التطوير.
- ABCI/gRPC: الاتصال بـ `mainnet-beta` RPC rate-limited. devnet OK.
