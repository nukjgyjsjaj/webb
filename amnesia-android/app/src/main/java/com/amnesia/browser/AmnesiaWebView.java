package com.amnesia.browser;

import android.content.Context;
import android.util.AttributeSet;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.webkit.ValueCallback;
import android.widget.FrameLayout;

public class AmnesiaWebView extends WebView {

    public AmnesiaWebView(Context context) {
        super(context);
        initialize();
    }

    public AmnesiaWebView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initialize();
    }

    public AmnesiaWebView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        initialize();
    }

    private void initialize() {
        WebSettings settings = getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(false);
        settings.setDatabaseEnabled(false);
        settings.setCacheMode(WebSettings.LOAD_NO_CACHE);
        settings.setAppCacheEnabled(false);
        settings.setSaveFormData(false);
        settings.setSavePassword(false);
        settings.setBlockNetworkImage(false);
        settings.setLoadsImagesAutomatically(true);
        settings.setAllowContentAccess(false);
        settings.setAllowFileAccess(false);
        settings.setAllowFileAccessFromFileURLs(false);
        settings.setAllowUniversalAccessFromFileURLs(false);
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_NEVER_ALLOW);
        settings.setUserAgentString(AmnesiaEngine.getSpoofedUserAgent());

        setWebViewClient(new WebViewClient() {
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url);
                return true;
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                super.onPageFinished(view, url);
                injectPrivacyShield(view, url);
            }

            @Override
            public void onPageCommitVisible(WebView view, String url) {
                super.onPageCommitVisible(view, url);
                injectPrivacyShield(view, url);
            }
        });
    }

    private void injectPrivacyShield(WebView view, String url) {
        if (!AmnesiaEngine.isInitialized()) return;

        String profileJson = AmnesiaEngine.getProfileJson();
        String sessionId = AmnesiaEngine.getSessionId();

        String script = "(function() {" +
            "window.__amnesia_profile = " + profileJson + ";" +
            "window.__amnesia_seed = " + (sessionId != null ? sessionId.hashCode() : 0) + ";" +
            "(function() {" +
                "if (window.__amnesia_injected) return;" +
                "window.__amnesia_injected = true;" +
                "const profile = window.__amnesia_profile || {};" +
                "const seed = window.__amnesia_seed || 0;" +
                "function hashSeed(data) {" +
                    "let h = seed;" +
                    "for (let i = 0; i < data.length; i++) {" +
                        "h = ((h << 5) - h + data.charCodeAt(i)) | 0;" +
                    "}" +
                    "return Math.abs(h);" +
                "}" +
                "function consistentRandom(offset) {" +
                    "return (hashSeed(offset) % 1000) / 1000.0;" +
                "}" +
                "['userAgent','platform','oscpu','hardwareConcurrency','deviceMemory','language','languages','cookieEnabled','doNotTrack','maxTouchPoints','vendor','vendorSub','product','productSub','appVersion','appName','appCodeName'].forEach(prop => {" +
                    "Object.defineProperty(navigator, prop, {get: () => profile[prop] || navigator[prop], configurable: false, enumerable: true});" +
                "});" +
                "['width','height','availWidth','availHeight','colorDepth','pixelDepth','orientation'].forEach(prop => {" +
                    "Object.defineProperty(screen, prop, {get: () => profile['screen_' + prop] || screen[prop], configurable: false, enumerable: true});" +
                "});" +
                "Object.defineProperty(window, 'devicePixelRatio', {get: () => profile.device_pixel_ratio || window.devicePixelRatio, configurable: false, enumerable: true});" +
                "if (profile.performance_spoof_enabled) {" +
                    "const origNow = performance.now.bind(performance);" +
                    "performance.now = function() {" +
                        "const noise = (consistentRandom('perf') - 0.5) * 1000 * (profile.perf_noise_factor || 0.05);" +
                        "return origNow() + noise;" +
                    "};" +
                "}" +
                "if (profile.timezone_name) {" +
                    "const origDate = Date;" +
                    "const tzOffset = profile.timezone_offset_minutes || 0;" +
                    "Date = function(...args) {" +
                        "const d = new origDate(...args);" +
                        "const utc = d.getTime() + (d.getTimezoneOffset() * 60000);" +
                        "return new Date(utc + (tzOffset * 60000));" +
                    "};" +
                    "Date.now = function() {" +
                        "const now = new origDate();" +
                        "const utc = now.getTime() + (now.getTimezoneOffset() * 60000);" +
                        "return utc + (tzOffset * 60000);" +
                    "};" +
                    "Date.prototype = origDate.prototype;" +
                "}" +
                "if (profile.canvas_noise_enabled && profile.canvas_noise_amplitude) {" +
                    "const origToDataURL = HTMLCanvasElement.prototype.toDataURL;" +
                    "HTMLCanvasElement.prototype.toDataURL = function(t, q) {" +
                        "const data = origToDataURL.call(this, t, q);" +
                        "const noisy = data.split('').map((c, i) => {" +
                            "const noise = Math.floor(consistentRandom('canvas_' + i) * profile.canvas_noise_amplitude * 255);" +
                            "const code = c.charCodeAt(0);" +
                            "return String.fromCharCode(Math.min(255, Math.max(0, code + ((i % 2 === 0) ? noise : -noise))));" +
                        "}).join('');" +
                        "return noisy;" +
                    "};" +
                "}" +
                "if (profile.webgl_spoof_enabled && profile.webgl_renderer) {" +
                    "const getParam = WebGLRenderingContext.prototype.getParameter;" +
                    "WebGLRenderingContext.prototype.getParameter = function(param) {" +
                        "if (param === 37446) return profile.webgl_renderer;" +
                        "if (param === 37445) return profile.webgl_vendor;" +
                        "return getParam.call(this, param);" +
                    "};" +
                "}" +
                "const OrigRTC = window.RTCPeerConnection || window.webkitRTCPeerConnection;" +
                "if (OrigRTC && profile.webrtc_leak_prevention) {" +
                    "window.RTCPeerConnection = function(...args) {" +
                        "const pc = new OrigRTC(...args);" +
                        "const origCreateOffer = pc.createOffer.bind(pc);" +
                        "pc.createOffer = function(...a) {" +
                            "return origCreateOffer(...a).then(offer => {" +
                                "if (offer && offer.sdp) {" +
                                    "offer.sdp = offer.sdp.replace(/^a=.*ice-options.*$/m, '');" +
                                    "offer.sdp = offer.sdp.replace(/(c=IN IP4 )(\\S+)/g, 'c=IN IP4 0.0.0.0');" +
                                "}" +
                                "return offer;" +
                            "});" +
                        "};" +
                        "return pc;" +
                    "};" +
                    "window.RTCPeerConnection.prototype = OrigRTC.prototype;" +
                "}" +
                "window.dispatchEvent(new Event('amnesia-ready'));" +
            "})();" +
        "})();";

        try {
            view.evaluateJavascript(script, null);
        } catch (Exception e) {
            view.loadUrl("javascript:" + script);
        }
    }

    // ... existing methods ...
}
