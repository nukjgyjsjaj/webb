package com.amnesia.browser;

import android.content.Context;
import android.util.Log;
import android.webkit.WebView;
import android.webkit.WebSettings;

public class ApiShieldInjector {

    private static final String TAG = "ApiShieldInjector";
    private static boolean injected = false;

    public static synchronized void injectOnce(Context ctx, WebView webView) {
        if (injected || webView == null) return;

        try {
            String profileJson = AmnesiaEngine.getProfileJson();
            String sessionId = AmnesiaEngine.getSessionId();

            String script = buildInjectionScript(profileJson, sessionId);
            webView.evaluateJavascript(script, null);
            injected = true;
            Log.i(TAG, "API Shield injected");
        } catch (Exception e) {
            Log.e(TAG, "Injection failed", e);
        }
    }

    public static synchronized void resetInjection() {
        injected = false;
    }

    private static String buildInjectionScript(String profileJson, String sessionId) {
        StringBuilder sb = new StringBuilder();
        sb.append("(function() {");
        sb.append("if (window.__amnesia_injected) return;");
        sb.append("window.__amnesia_injected = true;");
        sb.append("window.__amnesia_profile = ").append(profileJson).append(";");
        sb.append("window.__amnesia_seed = ").append(sessionId != null ? sessionId.hashCode() : 0).append(";");

        // Navigator overrides
        sb.append("Object.defineProperty(navigator, 'userAgent', {get: function() { return window.__amnesia_profile.user_agent || navigator.userAgent; }, configurable: false});");
        sb.append("Object.defineProperty(navigator, 'platform', {get: function() { return window.__amnesia_profile.platform || 'Linux armv8l'; }, configurable: false});");

        // Screen overrides
        sb.append("Object.defineProperty(screen, 'width', {get: function() { return window.__amnesia_profile.screen_width || screen.width; }, configurable: false});");
        sb.append("Object.defineProperty(screen, 'height', {get: function() { return window.__amnesia_profile.screen_height || screen.height; }, configurable: false});");

        // WebGL overrides
        sb.append("try {");
        sb.append("var getGLParameter = WebGLRenderingContext.prototype.getParameter;");
        sb.append("WebGLRenderingContext.prototype.getParameter = function(param) {");
        sb.append("if (param === 37446) return window.__amnesia_profile.webgl_renderer || 'ANGLE (Qualcomm, Adreno (TM) 750, OpenGL ES 3.2)';");
        sb.append("if (param === 37445) return window.__amnesia_profile.webgl_vendor || 'Qualcomm';");
        sb.append("return getGLParameter.call(this, param);");
        sb.append("};");
        sb.append("} catch(e) {}");

        // Canvas noise
        sb.append("try {");
        sb.append("var origToDataURL = HTMLCanvasElement.prototype.toDataURL;");
        sb.append("HTMLCanvasElement.prototype.toDataURL = function(type, quality) {");
        sb.append("var data = origToDataURL.call(this, type, quality);");
        sb.append("if (window.__amnesia_profile.canvas_noise_amplitude) {");
        sb.append("var noise = window.__amnesia_profile.canvas_noise_amplitude;");
        sb.append("var result = '';");
        sb.append("for (var i = 0; i < data.length; i++) {");
        sb.append("var c = data.charCodeAt(i);");
        sb.append("var n = (Math.random() - 0.5) * noise * 255;");
        sb.append("result += String.fromCharCode(Math.min(255, Math.max(0, c + Math.floor(n))));");
        sb.append("}");
        sb.append("return result;");
        sb.append("}");
        sb.append("return data;");
        sb.append("};");
        sb.append("} catch(e) {}");

        // Timezone spoofing
        sb.append("try {");
        sb.append("var tz = window.__amnesia_profile.timezone_name || 'UTC';");
        sb.append("var offset = window.__amnesia_profile.timezone_offset_minutes || 0;");
        sb.append("var origDate = Date;");
        sb.append("Date = function() { var d = new origDate(); var utc = d.getTime() + d.getTimezoneOffset() * 60000; return new Date(utc + offset * 60000); };");
        sb.append("Date.now = function() { var d = new origDate(); var utc = d.getTime() + d.getTimezoneOffset() * 60000; return utc + offset * 60000; };");
        sb.append("Date.prototype = origDate.prototype;");
        sb.append("} catch(e) {}");

        // WebRTC leak prevention
        sb.append("try {");
        sb.append("if (window.RTCPeerConnection || window.webkitRTCPeerConnection) {");
        sb.append("var OrigRTC = window.RTCPeerConnection || window.webkitRTCPeerConnection;");
        sb.append("window.RTCPeerConnection = function() {");
        sb.append("var pc = new OrigRTC();");
        sb.append("var origCreateOffer = pc.createOffer.bind(pc);");
        sb.append("pc.createOffer = function() {");
        sb.append("return origCreateOffer().then(function(offer) {");
        sb.append("if (offer && offer.sdp) {");
        sb.append("offer.sdp = offer.sdp.replace(/^a=.*ice-options.*$/m, '');");
        sb.append("offer.sdp = offer.sdp.replace(/(c=IN IP4 )(\\S+)/g, 'c=IN IP4 0.0.0.0');");
        sb.append("}");
        sb.append("return offer;");
        sb.append("});");
        sb.append("};");
        sb.append("return pc;");
        sb.append("};");
        sb.append("window.RTCPeerConnection.prototype = OrigRTC.prototype;");
        sb.append("}");
        sb.append("} catch(e) {}");

        // Disable WebUSB/WebBluetooth/WebMIDI
        sb.append("try { Object.defineProperty(navigator, 'usb', {get: function() { return undefined; }, configurable: false}); } catch(e) {}");
        sb.append("try { Object.defineProperty(navigator, 'bluetooth', {get: function() { return undefined; }, configurable: false}); } catch(e) {}");
        sb.append("try { Object.defineProperty(navigator, 'midi', {get: function() { return undefined; }, configurable: false}); } catch(e) {}");

        sb.append("window.dispatchEvent(new Event('amnesia-ready'));");
        sb.append("})();");

        return sb.toString();
    }
}
