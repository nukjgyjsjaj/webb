# الورقات البحثية - تصميم متصفح ويب مقاوم للبصمة الرقمية

## الورقة 1: تصميم متصفح ويب مقاوم للبصمة الرقمية بآلية إعادة ضبط ذاتية كاملة

### الملخص التنفيذي
تصميم معماري لمتصفح ويب جديد يعتمد على مبدأ "النسيان التام" (Complete Amnesia)، حيث يُعاد إنشاء بصمة رقمية جديدة ومتناسقة بالكامل مع كل جلسة تصفح، مع محو جميع البيانات السابقة. يُبنى المتصفح على تعديلات على مستوى كود C++ المصدر.

### القسم 1: مقدمة في بصمة المتصفح الرقمية
- تعريف بصمة المتصفح كبديل للكوكيز
- 170+ إشارة رقمية مجمعة دون علم المستخدم
- يمكن تحقيق تميز بنسبة >99% بين 100,000 مستخدم

### القسم 2: التهديد
- سكريبات تتبع مدمجة في الصفحة
- شبكات إعلانية
- أنظمة مكافحة الاحتيال
- أدوات تحليل الزوار

### القسم 3: تحليل شامل للإشارات الرقمية (171 إشارة)

#### 3.1 معلومات الجهاز ونظام التشغيل (6 إشارات)
- Device Type / Model
- Operating System
- True Operating System Core
- Browser
- Browser Engine
- True Browser Core

#### 3.2 الشبكة والموقع (13 إشارة)
- IP Address (WAN)
- Tor/VPN/Proxy IP Address
- Hostname
- Country/Region/City
- Latitude & Longitude
- ISP
- Nameservers
- Local IP Address (LAN) - WebRTC
- Connection Status

#### 3.3 التاريخ والمنطقة الزمنية (6 إشارات)
- System Time
- System Time Zone
- Local Time
- Local Time Zone
- System DST Status
- Local DST Status

#### 3.4 بصمة القماش Canvas (3 إشارات)
- Canvas Support
- Canvas Fingerprinting
- Canvas Fingerprinting Status

#### 3.5 بصمة الصوت AudioContext (2 إشارة)
- AudioContext Support
- AudioContext Fingerprinting

#### 3.6 وكيل المستخدم ورؤوس الطلب (33 إشارة)
- User-Agent
- Accept، Accept-Encoding، Accept-Language
- Cache-Control، Connection، Cookie
- Device-Memory، Downlink، DPR، ECT، Host، Priority
- Sec-CH-* (20 رأس)
- Sec-Fetch-* (4 رؤوس)
- Upgrade-Insecure-Requests، Viewport-Width، architecture، bitness

#### 3.7 معلومات العتاد والأجهزة الطرفية (22 إشارة)
- Graphics Card / Driver (WebGL)
- CPU Architecture، Number of Cores
- Memory (RAM)
- Battery Level/Charging/Time Remaining
- Bluetooth Support/Adapter
- Device Orientation (6)
- Device Motion (3)
- Speakers/Microphones/Cameras

#### 3.8 طريقة التفاعل (13 إشارة)
- Device Orientation (الزوايا الثلاث)
- Mouse Support
- Touchscreen Support
- Fonts، SpeechSynthesis، Speech Voices
- Browser Plugins، Firefox Extensions، Java

#### 3.9 إعدادات المتصفح والأمان (20 إشارة)
- HTTP Version، TLS Protocol، TLS Cipher
- Cookies Enabled
- History Entries Count
- Page Visibility (3)
- Private Browsing Mode، Do Not Track
- JavaScript، WebRTC، WebGL
- WebSocket Support، Extensions، WebSocket Connections

#### 3.10 شاشة العرض (12 إشارة)
- Screen Orientation، Resolution
- Aspect Ratio
- Device Pixel Ratio
- Color Depth
- Browser Window Size (Outer، Inner، Full Screen)
- Browser Build Number / Identifier
- Page Scroll Position

#### 3.11 التخزين والتفاعل (15 إشارة)
- IndexedDB، Local Storage، Session Storage
- Page Referrer
- Page Request Date & Time
- Performance Metrics (7)

#### 3.12 الإشارات الناشئة (24 إشارة)
- WebUSB (4)
- WebBluetooth (3)
- WebGPU (4)
- MIDI devices (4)
- Sensors (4)
- WebCodecs، SharedArrayBuffer، Cross-Origin Isolation، User Activation Heuristics، TLS Fingerprinting

### القسم 4: تحليل نقاط الضعف في الحلول الحالية
- امتدادات JavaScript: قابلية الكشف، عدم التناسق
- التوزيع العشوائي: مفارقة العشوائية
- حجب APIs بالكامل: تعطيل وظائف مشروعة

### القسم 5: الهندسة المعمارية المقترحة

#### 5.1 المبادئ الأساسية
1. النسيان التام عند الإغلاق
2. إعادة إنشاء البصمة بالكامل عند كل تشغيل
3. التعديل على مستوى C++
4. التوحيد ضمن الجلسة
5. عدم استخدام آلية عشوائية خالصة

#### 5.2 الهيكل المعماري
```
طبقة إدارة الجلسة (Session)
    ↓
محرك التعديل على مستوى C++
    ↓
محرك التخزين المؤقت للجلسة
    ↓
منظومة التنظيف (Cleanup)
```

#### 5.3 آلية توليد البصمة المتسقة
- Session ID عشوائي (128-bit)
- PRNG كبذرة
- قيم ضمن النطاق الطبيعي

#### 5.4 التعديلات على مستوى C++
- Canvas: تعديل toDataURL()
- WebGL: تعديل WEBGL_debug_renderer_info
- AudioContext: تعديل مخرجات المعالجة
- Navigator: تعديل جميع الخصائص
- Screen: تعديل الأبعاد والدقة
- HTTP Headers: تعديل الرؤوس المرسلة

### القسم 6: الحماية لكل فئة من الإشارات
- معلومات الجهاز: تزييف UA، Sec-CH-UA، النظام الأساسي
- الشبكة: إخفاء IP (Tor/VPN)، منع تسرب WebRTC
- المنطقة الزمنية: توحيد المنطقة الزمنية
- Canvas: تشويش متناسق
- AudioContext: تشويش متناسق
- WebGL/WebGPU: تزييف WEBGL_debug_renderer_info
- الخطوط: تزييف قائمة الخطوط
- الأجهزة الطرفية: تعطيل WebUSB/WebBluetooth/WebMIDI
- الأداء: تشويش التوقيت
- التخزين: حذف الكوكيز عند الإغلاق

### القسم 7: آلية إعادة الضبط الذاتي الكامل
- مدير الجلسة (Session Manager)
- مولد البصمة (Fingerprint Generator)
- مدير التخزين المؤقت (Ephemeral Storage Manager)
- منظومة التنظيف (Cleanup System)

### القسم 8: مقارنة مع المتصفحات الحالية
- Tor Browser: توحيد البصمة، إعادة ضبط كامل، تعديل C++ جزئي، بطيء
- Brave: تشويش عشوائي، لا إعادة ضبط
- Firefox (RFP): توحيد بعض الإشارات، ثابتة
- المتصفح المقترح: يجمع كل الميزات

### القسم 9: متطلبات التنفيذ التقنية
- محرك Blink (Chromium)
- تعديلات في:
  - third_party/blink/renderer/modules/canvas/
  - third_party/blink/renderer/modules/webgl/
  - third_party/blink/renderer/modules/webaudio/
  - third_party/blink/renderer/core/frame/navigator.cc
  - third_party/blink/renderer/core/frame/local_dom_window.cc
  - net/http/http_util.cc
  - components/network_session_configurator/

### القسم 10: التحديات والحلول
- كشف التعديلات: تعديل على مستوى C++
- التناقض بين الإشارات: PRNG واحد
- تطبيقات الويب المعقدة: وضع "التوافق"
- كشف VPN/Tor: تكامل مع شبكات موثوقة

### القسم 11: البنية التحتية
- واجهة المستخدم
- محرك التصفح (Blink)
- مدير الجلسة
- مدير الإضافات
- مدير التخزين المؤقت
- مدير التنظيف
- طبقة الشبكة (Tor/VPN مدمج)

### القسم 12: خطة التطوير (24 شهر)
- المرحلة 1 (3-6 شهر): Fork من Chromium، مدير الجلسة، 50 إشارة
- المرحلة 2 (6-12 شهر): 170+ إشارة، التنظيف الكامل
- المرحلة 3 (12-18 شهر): تحسين الأداء، Tor/VPN مدمج
- المرحلة 4 (18-24 شهر): إصدار مستقر

### القسم 13: تقييم الأمن والخصوصية
- نقاط القوة: عدم التتبع، عدم التخزين الدائم، مقاومة البصمة
- نقاط الضعف: عنوان IP، السلوك البشري، شهادات TLS

---

## الورقة 2: حلول متكاملة للنقاط المتبقية في مقاومة البصمة الرقمية

### الملخص التنفيذي
معالجة عنوان IP وبصمة TLS كحلول متكاملة:
1. دمج نظام VLESS مع دوران تلقائي للوكلاء
2. تزوير بصمة TLS باستخدام uTLS

### القسم 1: عنوان IP - تحليل المشكلة
- الرابط الوحيد الذي يربط الجلسة بموقع جغرافي
- يمكن استخدامه لربط الجلسات حتى مع تغيير جميع الإشارات الأخرى

### القسم 2: الحل المقترح - VLESS مع دوران تلقائي
#### 2.1 ما هو VLESS؟
بروتوكول وكيل خفيف الوزن مبني على Xray-core

#### 2.2 آلية العمل
```
جامع الروابط (VLESS Collector)
    ↓
مدير الروابط النشطة (Active Link Manager)
    ↓
عميل Xray-core المدمج
    ↓
وكيل SOCKS5 محلي
```

#### 2.3 مصادر الروابط
- vless-proxy-list (كل 4 ساعات)
- Proxy-List (كل ساعة)
- FreeProxyList (كل 10 دقائق)
- Vless Collector (مستمر)
- Freedom-V2Ray (كل ساعتين)

#### 2.4 آلية التحديث والدوران
- سحب قائمة كل 15 دقيقة
- اختبار 3-5 روابط
- اختيار عشوائي من الصالحة
- تبديل تلقائي عند الفشل

### القسم 3: بصمة TLS (شهادات النقل)
#### 3.1 ما هي بصمة TLS؟
- ClientHello في نص واضح
- تحتوي على: TLS version، Cipher Suites، Extensions، Elliptic Curves

#### 3.2 خوارزميات الكشف
- JA3 (2017-2021): 32 حرفاً
- JA4 (2023-2026): الجيل التالي

### القسم 4: الحل المقترح - تزوير بصمة TLS
#### 4.1 الأدوات المتاحة
- uTLS (Go)
- TLS-Chameleon (Python)
- JA3Cloak (Go/C#)
- curl-impersonate (Python/C)

#### 4.2 تقنية uTLS
- التقليد (Parroting): HelloChrome_Auto
- البصمة العشوائية: HelloRandomized
- إعادة استخدام بصمة ناجحة

#### 4.3 mTLS Roller
- تجرب بصمات مختلفة حتى تعمل
- تعيد استخدام البصمة الناجحة

### القسم 5: التكامل الكامل
#### 5.1 دورة حياة الجلسة
1. توليد معرف جلسة فريد
2. سحب واختبار روابط VLESS
3. اختيار ملف تعريف TLS
4. توليد جميع الإشارات (170+)
5. بدء التصفح
6. إغلاق: حذف جميع البيانات

### القسم 7: التوصيات
- اعتماد uTLS كطبقة TLS أساسية
- دمج Xray-core مع مدير الجلسة
- سحب قوائم VLESS من مستودعات متعددة
- اختبار صلاحية الروابط قبل الاستخدام
- ربط بصمة TLS بمعرف الجلسة

---

## الورقة 3: آلية التهيئة المسبقة للجلسة (Session Pre-Initialization)

### الملخص التنفيذي
آلية تضمن تناسق كامل للبصمة الرقمية المزورة من خلال التهيئة المسبقة (30-60 ثانية) قبل بدء التصفح.

### القسم 1: المشكلة الجوهرية
#### 1.1 التناقض الداخلي
- أنظمة الكشف تتحقق تبادلياً من عشرات الإشارات
- مثال: UA يقول Android لكن WebGL shows NVIDIA Windows GPU

#### 1.2 "التناسق الزائف"
- UA/النواة غير متطابق
- تسرب العتاد
- بيئات محمولة مزيّفة

#### 1.3 لماذا التوزيع العشوائي غير كافٍ؟
- يخلق تناقضات
- يجعل المستخدم أكثر قابلية للكشف

### القسم 2: مبدأ التناسق المطلق
- "One seed → one internally-coherent browser identity"
- جميع الإشارات من بذرة واحدة
- كل إشارة تبدو طبيعية ضمن سياق الإشارات الأخرى

### القسم 3: الهندسة المعمارية

#### 3.1 مراحل التهيئة (30-60 ثانية)
1. توليد البذرة (Seed Generation) - < 1 ثانية
2. سحب واختبار روابط VLESS - 5-15 ثانية
3. توليد الملف التعريفي الكامل - < 1 ثانية
4. اختبار التناسق - 5 ثواني
5. تهيئة محرك التصفح - 10-30 ثانية
6. إعلام المستخدم - < 1 ثانية

### القسم 4: توليد البصمة المتسقة
#### 4.1 PRNG حتمي
- Mulberry32 أو PCG أو SplitMix64
- نفس البذرة → نفس البصمة

#### 4.2 الهيكل البياني الحتمي (Deterministic DAG)
```
البذرة (Seed)
    ↓
نوع الجهاز / نظام التشغيل / إصدار المتصفح
    ↓
دقة الشاشة / GPU / عدد الأنوية
```

#### 4.3 مبدأ "عدم التناقض"
- الوكيل في ألمانيا → منطقة زمنية Europe/Berlin → لغة de-DE
- Redmi Note 8T → دقة شاشة وGPU وأنوية متوافقة
- بصمة TLS Chrome → UA مطابق

### القسم 5: تناسق الشبكة والموقع
- IP ← عنوان الوكيل (VLESS)
- البلد/المنطقة/المدينة ← GeoIP للوكيل
- المنطقة الزمنية ← مُشتقة من موقع الوكيل
- اللغة ← مُشتقة من موقع الوكيل
- ISP ← ASN للوكيل

### القسم 6: تناسق TLS
- اختيار ملف تعريف TLS بناءً على UA
- استخدام uTLS 1.8.1+ لتجنب CVE-2026-27017
- تناسق HTTP/2 Settings

### القسم 7: تناسق طبقة التطبيقات
- Canvas/WebGL/AudioContext: ضوضاء حتمية (Deterministic Noise)
- Fonts/Screen/Navigator: متوافقة مع الملف التعريفي

### القسم 8: اختبار التناسق قبل التصفح
- اختبار UA/TLS
- اختبار UA/WebGL
- اختبار الموقع/المنطقة الزمنية
- اختبار الجهاز/الشاشة
- 0 تناقضات: تصفح فوري
- 1-2 تناقضات: إعادة توليد
- >2 تناقضات: إعادة توليد البصمة بالكامل

### القسم 9: التحديات
- تغير روابط VLESS: سحب كل 15 دقيقة
- JA4: تزوير معقد
- ثغرات uTLS: استخدام 1.8.1+
- كشف التناقض: ربط TLS بUA
- فشل الوكيل: تبديل تلقائي
- فترة التهيئة الطويلة: عمليات بالتوازي

### القسم 11: التوصيات
- اعتماد PRNG حتمي إلزامي
- اشتقاق المنطقة الزمنية واللغة من الوكيل إلزامي
- مطابقة بصمة TLS مع UA عبر uTLS إلزامي
- اختبارات تناسق ذاتية إلزامي
- تحديث قوائم VLESS كل 15 دقيقة موصى به
