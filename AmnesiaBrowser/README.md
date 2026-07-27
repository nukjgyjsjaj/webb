# Amnesia Browser
متصفح ويب مقاوم للبصمة الرقمية بآلية إعادة ضبط ذاتية كاملة - "النسيان التام"

## المبدأ الأساسي
- تصميم متصفح ويب جديد يعتمد على مبدأ "النسيان التام" (Complete Amnesia)
- إعادة إنشاء بصمة رقمية جديدة ومتناسقة بالكامل مع كل جلسة تصفح
- محو جميع البيانات السابقة عند الإغلاق
- تعديلات على مستوى كود C++ المصدر (غير قابل للكشف مقارنة بإضافات JavaScript)

## الهندسة المعمارية
```
طبقة إدارة الجلسة (Session)
    ↓
محرك التعديل على مستوى C++
    ↓
محرك التخزين المؤقت للجلسة
    ↓
منظومة التنظيف (Cleanup)
```

## 171 إشارة رقمية مغطاة
1. Device/OS (6) | 2. Network/Location (13) | 3. Time/Timezone (6)
4. Canvas (3) | 5. AudioContext (2) | 6. User-Agent & Headers (33)
7. Hardware/Peripherals (22) | 8. Interaction (13) | 9. Browser Settings (20)
10. Screen/Display (12) | 11. Storage/Performance (15) | 12. Emerging Vectors (24)

## المكونات الأساسية
1. **Session Manager**: إنشاء معرف جلسة 128-bit، تخزين ذاكري فقط
2. **Fingerprint Generator**: توليد 170+ قيمة من بذرة واحدة (PRNG حتمي)
3. **Ephemeral Storage Manager**: تخزين مؤقت في RAM/قرص مشفر
4. **Cleanup System**: حذف جميع البيانات عند الإغلاق
5. **TLS Manager**: تزوير بصمة JA3/JA4 عبر uTLS 1.8.1+
6. **VLESS Manager**: جمع روابط، اختبار صلاحية، دوران تلقائي
7. **Xray-core Integration**: عميل VLESS مدمج مع واجهة برمجة تطبيقات داخلية
8. **Consistency Engine**: Deterministic DAG (48 قاعدة) لضمان تناسق كامل
9. **Device Profiles Database**: 100+ طراز جهاز حقيقي
10. **Browser Profiles Library**: 45+ ملف تعريف متصفح
11. **Pre-Browsing Validation**: اختبارات تناسق ذاتية (محاكاة CreepJS)
12. **GeoIP System**: قاعدة بيانات محلية لاشتقاق الموقع من الوكيل
13. **Deterministic Noise**: ضوضاء حتمية على Canvas/WebGL/AudioContext
14. **UI Framework**: شاشة تهيئة 30-60 ثانية مع تقدم العملية

## متطلبات البناء
- depot_tools
- GN/Ninja
- LLVM/Clang
- Python 3.9+
- Go 1.21+ (لـ uTLS)
- CMake (لـ Xray-core)

## خطة التطوير (24 شهر)
- المرحلة 1 (3-6 شهر): Fork Chromium، مدير الجلسة، 50 إشارة
- المرحلة 2 (6-12 شهر): 170+ إشارة، التنظيف الكامل
- المرحلة 3 (12-18 شهر): تحسين الأداء، Tor/VPN مدمج، UI
- المرحلة 4 (18-24 شهر): إصدار مستقر

## الإصدار
- Alpha: بعد اجتياز 90% من اختبارات الكشف
- Beta: مع 1000+ مستخدم
- Stable: بعد اجتياز جميع الاختبارات والتدقيق الأمني

## الترخيص
[سيتم تحديده]
