# Amnesia Browser - الحالة الحالية والخطة الجديدة

## تاريخ المشروع

### المرحلة 1: البحث والتخطيط (مكتمل)
- قراءة 3 أوراق بحثية عن anti-fingerprinting و VLESS و session pre-initialization
- إنشاء قاعدة بيانات 171 إشارة fingerprinting
- تصميم архитекورة كاملة

### المرحلة 2: البناء الأول (إنتلينت توقف)
- بناء C++ core modules (session, fingerprint, TLS, vless, geoip, cleanup)
- إنشاء 48 ملف مصدر
- بناء ناجح لـ 10 مكتبات + executable
- **المشكلة:** تم البناء لـ Linux/x86_64 فقط، ليس Android

## المشاكل في المشروع الحالي

### 1. ليس Android-native
- المشروع الحالي يعمل كـ desktop Linux app
- لا يوجد Android.mk أو AndroidManifest.xml
- لا يوجد gradle.build
- لا يوجد ملف APK output
- يعتمد على glibc لا ينجح على Android

### 2. البنية خاطئة
- تم بناءه كإضافات/layers فوق نظام موجود
- لا يوجد engine من الصفر
- Chromium/Blink patches موجودة لكن غير مطبقة فعلياً
- uTLS و Xray-core مدمجين كـ C++ لكن يحتاجون عملية

### 3. ملفات غير ضرورية ضخمة
- 218.8KB من الملفات لا علاقة لها بـ Android
- Build artifacts في build/
- No actual APK
- dependencies غير منظمة

## المتطلبات الحقيقية (Confirmed)

### 1. Android APK حقيقي
- ملف .apk يعمل على Android 8.0+ (API 26+)
- لا يحتاج root
- يعمل كتطبيق عادي

### 2. Browser Engine من الصفر
- **ليس إضافات** على متصفح موجود
- **ليس patches** على Chromium/Blink
- Engine C++ مدمج مع Android NDK
- UI/UX خاصة بالتطبيق

### 3. Anti-fingerprinting حقيقي
- 171 إشارة fingerprinting مغطاة
- تناسق بين جميع الإشارات
- Device profiles حقيقية
- Browser profiles حقيقية
- Crypto/UA/JA3/JA4 spoofing

### 4. VLESS/Xray mدمج
- Proxy integration في الـ engine
- Auto-rotation
- No leaks

## الخطة الجديدة (Android-First)

### المرحلة 1: Android Core Foundation
```
amnesia-android/
├── app/
│   ├── src/main/
│   │   ├── java/ - Android UI layer
│   │   ├── cpp/ - C++ engine core (NDK)
│   │   ├── res/ - Resources
│   │   └── AndroidManifest.xml
│   └── build.gradle
├── cpp/
│   ├── engine/ - Browser engine from scratch
│   ├── anti_fp/ - Anti-fingerprinting
│   ├── tls/ - TLS/JA3/JA4 spoofing
│   ├── proxy/ - VLESS/Xray integration
│   ├── session/ - Session management
│   └── cleanup/ - Data cleanup
├── gradle/ - Gradle wrapper
└── build.gradle
```

### المرحلة 2: C++ Engine من الصفر
- Minimal WebKit/Blink embedding
- Custom network stack
- Native TLS with uTLS
- No Chromium dependency

### المرحلة 3: الهندسة العكسية والتطبيق
- 171 fingerprinting signals
- Clock skew, battery, motion sensors
- Canvas/WebGL/WebAudio fingerprints
- Hardware concurrency spoofing

### المرحلة 4: Proxy Integration
- VLESS protocol implementation
- Xray-core integration
- Auto-link rotation
- Leak prevention

### المرحلة 5: Testing & Building
- 5+ terminals testing
- CreepJS validation
- BrowserLeaks validation
- APK signing and optimization

## القواعد الذهبية

1. **لا晴 säkra** - لا أستخدم متصفحات كقاعدة
2. **كل شيء من الصفر** - C++ engine فقط
3. **Android أولاً** - كل قرار يراعي Android
4. **لا ملفات غير ضرورية** - تنظيف مستمر
5. **اختبار مع كل وحدة** - لا كود بدون test

## وقت البدء
- الآن: توثيق وتنظيف
- بعدها: بناء من الصفر
- النتيجة: ملف .apk واحد يعمل
