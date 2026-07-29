# Add project specific ProGuard rules here.
# You can control the set of applied configuration files using the
# proguardFiles setting in build.gradle.

# Keep native methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep JNI-exposed classes
-keep class com.amnesia.browser.AmnesiaEngine {
    native <methods>;
}

-keep class com.amnesia.browser.AmnesiaWebView {
    public *;
}

-keep class com.amnesia.browser.AmnesiaEngineService {
    public *;
}

# Keep AndroidX
-keep class androidx.** { *; }
-keep interface androidx.** { *; }

# Keep Material components
-keep class com.google.android.material.** { *; }

# Keep WebView client
-keep class android.webkit.WebViewClient { *; }
