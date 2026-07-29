package com.amnesia.browser;

import android.content.Context;
import android.util.Log;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class AmnesiaEngine {

    private static final String TAG = "AmnesiaEngine";
    private static boolean initialized = false;
    private static String spoofedUserAgent = null;
    private static String spoofedScreenResolution = null;
    private static String spoofedLanguage = null;
    private static String spoofedTimezone = null;
    private static ExecutorService executor;

    static {
        System.loadLibrary("amnesia_engine");
    }

    public static synchronized boolean initialize(Context context) {
        if (initialized) return true;

        try {
            boolean result = nativeInitialize();
            if (result) {
                initialized = true;
                applySpoofing();
                Log.i(TAG, "Amnesia engine initialized successfully");
                return true;
            }
        } catch (Exception e) {
            Log.e(TAG, "Failed to initialize engine", e);
        }
        return false;
    }

    public static synchronized void shutdown() {
        if (!initialized) return;
        try {
            nativeShutdown();
            initialized = false;
            Log.i(TAG, "Amnesia engine shut down");
        } catch (Exception e) {
            Log.e(TAG, "Error shutting down engine", e);
        }
    }

    public static void navigate(String url) {
        if (!initialized || url == null) return;
        try {
            nativeNavigate(url);
        } catch (Exception e) {
            Log.e(TAG, "Navigation error", e);
        }
    }

    public static String evaluateJavaScript(String script) {
        if (!initialized || script == null) return "";
        try {
            return nativeEvaluateJavaScript(script);
        } catch (Exception e) {
            Log.e(TAG, "JS evaluation error", e);
            return "";
        }
    }

    public static String getSpoofedUserAgent() {
        if (spoofedUserAgent == null) {
            spoofedUserAgent = nativeSpoofUserAgent();
        }
        return spoofedUserAgent;
    }

    public static String getSpoofedScreenResolution() {
        if (spoofedScreenResolution == null) {
            spoofedScreenResolution = nativeSpoofScreenResolution();
        }
        return spoofedScreenResolution;
    }

    public static String getSpoofedLanguage() {
        if (spoofedLanguage == null) {
            spoofedLanguage = nativeSpoofLanguage();
        }
        return spoofedLanguage;
    }

    public static String getSpoofedTimezone() {
        if (spoofedTimezone == null) {
            spoofedTimezone = nativeSpoofTimezone();
        }
        return spoofedTimezone;
    }

    public static void reinitializeWithNewProfile() {
        if (!initialized) return;
        try {
            applySpoofing();
            Log.i(TAG, "Profile reinitialized with new spoofing values");
        } catch (Exception e) {
            Log.e(TAG, "Reinit error", e);
        }
    }

    public static void applySpoofing() {
        try {
            nativeApplySpoofing();
            spoofedUserAgent = nativeSpoofUserAgent();
            spoofedScreenResolution = nativeSpoofScreenResolution();
            spoofedLanguage = nativeSpoofLanguage();
            spoofedTimezone = nativeSpoofTimezone();
        } catch (Exception e) {
            Log.e(TAG, "Spoofing error", e);
        }
    }

    public static String getJA3() {
        try {
            return nativeGetJA3();
        } catch (Exception e) {
            Log.e(TAG, "JA3 error", e);
            return "";
        }
    }

    public static String getJA4() {
        try {
            return nativeGetJA4();
        } catch (Exception e) {
            Log.e(TAG, "JA4 error", e);
            return "";
        }
    }

    public static boolean addVlessLink(String link) {
        if (!initialized || link == null) return false;
        try {
            return nativeAddVlessLink(link);
        } catch (Exception e) {
            Log.e(TAG, "Add VLESS error", e);
            return false;
        }
    }

    public static String getActiveVlessLink() {
        if (!initialized) return "";
        try {
            return nativeGetActiveLink();
        } catch (Exception e) {
            Log.e(TAG, "Get active link error", e);
            return "";
        }
    }

    public static boolean rotateVlessLink() {
        if (!initialized) return false;
        try {
            return nativeRotateLink();
        } catch (Exception e) {
            Log.e(TAG, "Rotate link error", e);
            return false;
        }
    }

    public static String createSession(int timeoutSeconds) {
        if (!initialized) return "";
        try {
            return nativeCreateSession(timeoutSeconds);
        } catch (Exception e) {
            Log.e(TAG, "Create session error", e);
            return "";
        }
    }

    public static boolean destroySession(String sessionId) {
        if (!initialized || sessionId == null) return false;
        try {
            return nativeDestroySession(sessionId);
        } catch (Exception e) {
            Log.e(TAG, "Destroy session error", e);
            return false;
        }
    }

    public static long getSessionSeed(String sessionId) {
        if (!initialized || sessionId == null) return 0;
        try {
            return nativeGetSessionSeed(sessionId);
        } catch (Exception e) {
            Log.e(TAG, "Get session seed error", e);
            return 0;
        }
    }

    public static boolean cleanupSession(String sessionId) {
        if (!initialized || sessionId == null) return false;
        try {
            return nativeCleanupSession(sessionId);
        } catch (Exception e) {
            Log.e(TAG, "Cleanup session error", e);
            return false;
        }
    }

    public static boolean clearAllCache() {
        if (!initialized) return false;
        try {
            return nativeClearCache();
        } catch (Exception e) {
            Log.e(TAG, "Clear cache error", e);
            return false;
        }
    }

    public static boolean isInitialized() {
        return initialized;
    }

    private native boolean nativeInitialize();
    private native void nativeShutdown();
    private native void nativeNavigate(String url);
    private native String nativeEvaluateJavaScript(String script);
    private native String nativeSpoofUserAgent();
    private native String nativeSpoofScreenResolution();
    private native String nativeSpoofLanguage();
    private native String nativeSpoofTimezone();
    private native void nativeApplySpoofing();
    private native String nativeGetJA3();
    private native String nativeGetJA4();
    private native boolean nativeAddVlessLink(String link);
    private native String nativeGetActiveLink();
    private native boolean nativeRotateLink();
    private native String nativeCreateSession(int timeout);
    private native boolean nativeDestroySession(String sessionId);
    private native long nativeGetSessionSeed(String sessionId);
    private native boolean nativeCleanupSession(String sessionId);
    private native boolean nativeClearCache();
}
