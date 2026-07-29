#include "injection_engine.h"
#include <jni.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "anti_fp/fingerprint_profile.h"

namespace amnesia {

InjectionEngine& InjectionEngine::GetInstance() {
    static InjectionEngine instance;
    return instance;
}

InjectionEngine::InjectionEngine() : initialized_(false) {}

InjectionEngine::~InjectionEngine() {
    Shutdown();
}

bool InjectionEngine::Initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (initialized_) return true;
    initialized_ = true;
    return true;
}

void InjectionEngine::Shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);
    initialized_ = false;
}

std::string InjectionEngine::GetAPIShieldBundle() const {
    return R"(
(function() {
    if (window.__amnesia_injected) return;
    window.__amnesia_injected = true;

    const profile = window.__amnesia_profile || {};
    const seed = window.__amnesia_seed || 0;

    function hashSeed(data) {
        let h = seed;
        for (let i = 0; i < data.length; i++) {
            h = ((h << 5) - h + data.charCodeAt(i)) | 0;
        }
        return Math.abs(h);
    }

    function consistentRandom(offset) {
        return (hashSeed(offset) % 1000) / 1000.0;
    }

    // Navigator overrides
    ['userAgent', 'platform', 'oscpu', 'hardwareConcurrency', 'deviceMemory',
     'language', 'languages', 'cookieEnabled', 'doNotTrack', 'maxTouchPoints',
     'vendor', 'vendorSub', 'product', 'productSub', 'appVersion', 'appName', 'appCodeName'].forEach(prop => {
        Object.defineProperty(navigator, prop, {
            get: () => profile[prop] || navigator[prop],
            configurable: false, enumerable: true
        });
    });

    // Screen overrides
    ['width', 'height', 'availWidth', 'availHeight', 'colorDepth', 'pixelDepth',
     'orientation'].forEach(prop => {
        Object.defineProperty(screen, prop, {
            get: () => profile['screen_' + prop] || screen[prop],
            configurable: false, enumerable: true
        });
    });

    // Window overrides
    Object.defineProperty(window, 'devicePixelRatio', {
        get: () => profile.device_pixel_ratio || window.devicePixelRatio,
        configurable: false, enumerable: true
    });

    // Performance timing noise
    if (profile.performance_spoof_enabled) {
        const origNow = performance.now.bind(performance);
        performance.now = function() {
            const noise = (consistentRandom('perf') - 0.5) * 1000 * (profile.perf_noise_factor || 0.05);
            return origNow() + noise;
        };
    }

    // Date/Timezone spoofing
    if (profile.timezone_name) {
        const origDate = Date;
        const origNow = Date.now;
        const tzOffset = profile.timezone_offset_minutes || 0;
        Date = function(...args) {
            const d = new origDate(...args);
            const utc = d.getTime() + (d.getTimezoneOffset() * 60000);
            return new Date(utc + (tzOffset * 60000));
        };
        Date.now = function() {
            const now = new origDate();
            const utc = now.getTime() + (now.getTimezoneOffset() * 60000);
            return utc + (tzOffset * 60000);
        };
        Date.prototype = origDate.prototype;
    }

    // Canvas noise
    if (profile.canvas_noise_enabled && profile.canvas_noise_amplitude) {
        const origToDataURL = HTMLCanvasElement.prototype.toDataURL;
        HTMLCanvasElement.prototype.toDataURL = function(type, quality) {
            const data = origToDataURL.call(this, type, quality);
            const noiseLevel = profile.canvas_noise_amplitude;
            const noisy = data.split('').map((c, i) => {
                const noise = Math.floor(consistentRandom('canvas_' + i) * noiseLevel * 255);
                const code = c.charCodeAt(0);
                const newCode = Math.min(255, Math.max(0, code + ((i % 2 === 0) ? noise : -noise)));
                return String.fromCharCode(newCode);
            }).join('');
            return noisy;
        };
    }

    // WebGL spoofing
    if (profile.webgl_spoof_enabled && profile.webgl_renderer) {
        const getParam = WebGLRenderingContext.prototype.getParameter;
        WebGLRenderingContext.prototype.getParameter = function(param) {
            if (param === 37446) return profile.webgl_renderer || 'ANGLE';
            if (param === 37445) return profile.webgl_vendor || 'Google Inc.';
            return getParam.call(this, param);
        };
    }

    // WebRTC leak prevention
    if (profile.webrtc_leak_prevention) {
        const OrigRTCPeerConnection = window.RTCPeerConnection || window.webkitRTCPeerConnection;
        if (OrigRTCPeerConnection) {
            window.RTCPeerConnection = function(...args) {
                const pc = new OrigRTCPeerConnection(...args);
                const origCreateOffer = pc.createOffer.bind(pc);
                pc.createOffer = function(...a) {
                    return origCreateOffer(...a).then(offer => {
                        if (offer && offer.sdp) {
                            offer.sdp = offer.sdp.replace(/^a=.*ice-options.*$/m, '');
                        }
                        return offer;
                    });
                };
                return pc;
            };
            window.RTCPeerConnection.prototype = OrigRTCPeerConnection.prototype;
        }
    }

    // Disable dangerous APIs
    ['WebUSB', 'WebBluetooth', 'WebMIDI', 'Sensor'].forEach(name => {
        if (profile['web' + name.toLowerCase() + '_enabled'] === false) {
            Object.defineProperty(window, name, {
                get: () => undefined,
                configurable: false, enumerable: true
            });
        }
    });

    // Storage policy
    const storageOverrides = profile.localStorage_enabled !== false;
    if (!storageOverrides) {
        Object.defineProperty(window, 'localStorage', {
            get: () => ({ getItem: () => null, setItem: () => {}, removeItem: () => {}, clear: () => {} }),
            configurable: false, enumerable: true
        });
    }

    // Font enumeration blocker
    if (profile.font_spoof_enabled && profile.font_list) {
        const origMeasure = CanvasRenderingContext2D.prototype.measureText;
        CanvasRenderingContext2D.prototype.measureText = function(text) {
            const m = origMeasure.call(this, text);
            Object.defineProperty(m, 'width', {
                get: () => m.width * 1.01 + (consistentRandom('font_' + text) * 2 - 1)
            });
            return m;
        };
    }

    // HTTP header overrides via Navigator / Performance
    if (profile.accept_language) {
        Object.defineProperty(navigator, 'languages', {
            get: () => profile.accept_language.split(','),
            configurable: false, enumerable: true
        });
    }

    // Sec-CH-UA overrides
    if (profile.sec_ch_ua) {
        try {
            const cs = document.createElement('meta');
            cs.name = 'chrome';
            cs.content = profile.sec_ch_ua;
            document.head.appendChild(cs);
        } catch (e) {}
    }

    window.__amnesia_profile = profile;
    window.__amnesia_seed = seed;
    window.dispatchEvent(new Event('amnesia-ready'));
})();
)";
    return script;
}

std::string InjectionEngine::BuildInjectionScript(const FingerprintProfile& profile) {
    std::stringstream ss;
    ss << "(function() {" << std::endl;
    ss << "if (window.__amnesia_injected) return;" << std::endl;
    ss << "window.__amnesia_injected = true;" << std::endl;
    ss << "window.__amnesia_seed = " << profile.session_id << ";" << std::endl;

    // Profile object
    ss << "window.__amnesia_profile = {" << std::endl;
    ss << "  user_agent: " << std::quoted(profile.user_agent) << "," << std::endl;
    ss << "  platform: " << std::quoted(profile.platform) << "," << std::endl;
    ss << "  screen_width: " << profile.screen_width << "," << std::endl;
    ss << "  screen_height: " << profile.screen_height << "," << std::endl;
    ss << "  device_pixel_ratio: " << std::fixed << std::setprecision(2) << profile.device_pixel_ratio << "," << std::endl;
    ss << "  hardware_concurrency: " << profile.hardware_concurrency << "," << std::endl;
    ss << "  device_memory_gb: " << profile.device_memory_gb << "," << std::endl;
    ss << "  timezone_name: " << std::quoted(profile.timezone_name) << "," << std::endl;
    ss << "  timezone_offset_minutes: " << profile.timezone_offset_minutes << "," << std::endl;
    ss << "  locale: " << std::quoted(std::string(profile.locale_language) + "_" + profile.locale_region) << "," << std::endl;
    ss << "  webgl_renderer: " << std::quoted(profile.webgl_renderer) << "," << std::endl;
    ss << "  webgl_vendor: " << std::quoted(profile.webgl_vendor) << "," << std::endl;
    ss << "  canvas_noise_amplitude: " << std::fixed << std::setprecision(3) << profile.canvas_noise_amplitude << "," << std::endl;
    ss << "  audio_noise_db: " << std::fixed << std::setprecision(3) << profile.audio_noise_db << "," << std::endl;
    ss << "  accept_language: " << std::quoted(profile.accept_language) << "," << std::endl;
    ss << "  connection_type: " << std::quoted(profile.connection_type) << "," << std::endl;
    ss << "  downlink_mbps: " << profile.downlink_mbps << "," << std::endl;
    ss << "  sec_ch_ua: " << std::quoted(profile.sec_ch_ua) << "," << std::endl;
    ss << "  sec_ch_ua_mobile: " << std::quoted(profile.sec_ch_ua_mobile) << "," << std::endl;
    ss << "  sec_ch_ua_platform: " << std::quoted(profile.sec_ch_ua_platform) << std::endl;
    ss << "};" << std::endl;

    // Inject the full API Shield bundle
    ss << GetAPIShieldBundle() << std::endl;
    ss << "})();" << std::endl;
    return ss.str();
}

std::string InjectionEngine::GetCanvasNoiseScript(double amplitude) const {
    std::stringstream ss;
    ss << "(function(){" << std::endl;
    ss << "const amp=" << std::fixed << std::setprecision(3) << amplitude << ";" << std::endl;
    ss << "const orig=HTMLCanvasElement.prototype.toDataURL;" << std::endl;
    ss << "HTMLCanvasElement.prototype.toDataURL=function(t,q){" << std::endl;
    ss << "  const d=orig.call(this,t,q);" << std::endl;
    ss << "  return d.split('').map((c,i)=>{const n=Math.floor(Math.random()*amp*255);const code=c.charCodeAt(0);return String.fromCharCode(Math.min(255,Math.max(0,code+((i%2===0)?n:-n)));}).join('');" << std::endl;
    ss << "};" << std::endl;
    ss << "})();" << std::endl;
    return ss.str();
}

std::string InjectionEngine::GetWebGLSpoofScript(const char* renderer, const char* vendor) const {
    std::stringstream ss;
    ss << "WebGLRenderingContext.prototype.getParameter=function(p){" << std::endl;
    ss << "  if(p===37446) return '" << renderer << "';" << std::endl;
    ss << "  if(p===37445) return '" << vendor << "';" << std::endl;
    ss << "  return getParameter.call(this,p);" << std::endl;
    ss << "};" << std::endl;
    return ss.str();
}

std::string InjectionEngine::GetAudioNoiseScript(double noise_db) const {
    std::stringstream ss;
    ss << "(function(){" << std::endl;
    ss << "const noise=" << std::fixed << std::setprecision(3) << noise_db << ";" << std::endl;
    ss << "const origGetByteFrequencyData=AnalyserNode.prototype.getByteFrequencyData;" << std::endl;
    ss << "AnalyserNode.prototype.getByteFrequencyData=function(arr){" << std::endl;
    ss << "  origGetByteFrequencyData.call(this,arr);" << std::endl;
    ss << "  for(let i=0;i<arr.length;i++){arr[i]=Math.min(255,Math.max(0,arr[i]+Math.floor((Math.random()-0.5)*255*noise)));}" << std::endl;
    ss << "};" << std::endl;
    ss << "})();" << std::endl;
    return ss.str();
}

std::string InjectionEngine::GetNavigatorSpoofScript(const FingerprintProfile& profile) const {
    std::stringstream ss;
    ss << "Object.defineProperty(navigator,'userAgent',{get:()=>" << std::quoted(profile.user_agent) << "});" << std::endl;
    ss << "Object.defineProperty(navigator,'platform',{get:()=>" << std::quoted(profile.platform) << "});" << std::endl;
    ss << "Object.defineProperty(navigator,'hardwareConcurrency',{get:()=>" << profile.hardware_concurrency << "});" << std::endl;
    ss << "Object.defineProperty(navigator,'deviceMemory',{get:()=>" << profile.device_memory_gb << "});" << std::endl;
    return ss.str();
}

std::string InjectionEngine::GetScreenSpoofScript(const FingerprintProfile& profile) const {
    std::stringstream ss;
    ss << "Object.defineProperty(screen,'width',{get:()=>" << profile.screen_width << "});" << std::endl;
    ss << "Object.defineProperty(screen,'height',{get:()=>" << profile.screen_height << "});" << std::endl;
    ss << "Object.defineProperty(screen,'availWidth',{get:()=>" << profile.screen_avail_width << "});" << std::endl;
    ss << "Object.defineProperty(screen,'availHeight',{get:()=>" << profile.screen_avail_height << "});" << std::endl;
    ss << "Object.defineProperty(screen,'colorDepth',{get:()=>" << profile.color_depth << "});" << std::endl;
    ss << "Object.defineProperty(screen,'pixelDepth',{get:()=>" << profile.pixel_depth << "});" << std::endl;
    ss << "Object.defineProperty(window,'devicePixelRatio',{get:()=>" << std::fixed << std::setprecision(2) << profile.device_pixel_ratio << "});" << std::endl;
    return ss.str();
}

std::string InjectionEngine::GetWebRTCLeakPreventionScript() const {
    return R"(
(function() {
    const OrigRTCPeerConnection = window.RTCPeerConnection || window.webkitRTCPeerConnection;
    if (!OrigRTCPeerConnection) return;
    window.RTCPeerConnection = function(...args) {
        const pc = new OrigRTCPeerConnection(...args);
        const origCreateOffer = pc.createOffer.bind(pc);
 and<span class='citation'>Add error boundaries</span>
        pc.createOffer = function(...a) {
            return origCreateOffer(...a).then(offer => {
                if (offer && offer.sdp) {
                    offer.sdp = offer.sdp.replace(/^a=.*ice-options.*$/m, '');
                    offer.sdp = offer.sdp.replace(/(c=IN IP4 )(\S+)/g, 'c=IN IP4 0.0.0.0');
                }
                return offer;
            });
        };
        return pc;
    };
    window.RTCPeerConnection.prototype = OrigRTCPeerConnection.prototype;
})();
)";
}

std::string InjectionEngine::GetTimezoneSpoofScript(const char* timezone, int offset_minutes) const {
    std::stringstream ss;
    ss << "(function(){" << std::endl;
    ss << "const tz=" << std::quoted(timezone) << ";" << std::endl;
    ss << "const offset=" << offset_minutes << ";" << std::endl;
    ss << "const origDate=Date;" << std::endl;
    ss << "Date=function(...args){" << std::endl;
    ss << "  const d=new origDate(...args);" << std::endl;
    ss << "  const utc=d.getTime()+(d.getTimezoneOffset()*60000);" << std::endl;
    ss << "  return new Date(utc+(offset*60000));" << std::endl;
    ss << "};" << std::endl;
    ss << "Date.now=function(){" << std::endl;
    ss << "  const now=new origDate();" << std::endl;
    ss << "  const utc=now.getTime()+(now.getTimezoneOffset()*60000);" << std::endl;
    ss << "  return utc+(offset*60000);" << std::endl;
    ss << "};" << std::endl;
    ss << "Date.prototype=origDate.prototype;" << std::endl;
    ss << "})();" << std::endl;
    return ss.str();
}

bool InjectionEngine::InjectIntoWebView(JNIEnv* env, jobject webview, const std::string& script) {
    if (!env || !webview || script.empty()) return false;

    jclass webview_cls = env->GetObjectClass(webview);
    if (!webview_cls) return false;

    jmethodID evaluate_js = env->GetMethodID(webview_cls,
        "evaluateJavascript", "(Ljava/lang/String;Landroid/webkit/ValueCallback;)V");
    if (!evaluate_js) return false;

    jstring js_str = env->NewStringUTF(script.c_str());
    env->CallVoidMethod(webview, evaluate_js, js_str, nullptr);
    env->DeleteLocalRef(js_str);
    env->DeleteLocalRef(webview_cls);

    return true;
}

std::string InjectionEngine::BuildNavigatorOverrides(const FingerprintProfile& profile) const {
    std::stringstream ss;
    ss << "Object.defineProperty(navigator,'userAgent',{get:()=>" << std::quoted(profile.user_agent) << "});";
    ss << "Object.defineProperty(navigator,'platform',{get:()=>" << std::quoted(profile.platform) << "});";
    ss << "Object.defineProperty(navigator,'hardwareConcurrency',{get:()=>" << profile.hardware_concurrency << "});";
    ss << "Object.defineProperty(navigator,'deviceMemory',{get:()=>" << profile.device_memory_gb << "});";
    return ss.str();
}

std::string InjectionEngine::BuildScreenOverrides(const FingerprintProfile& profile) const {
    std::stringstream ss;
    ss << "Object.defineProperty(screen,'width',{get:()=>" << profile.screen_width << "});";
    ss << "Object.defineProperty(screen,'height',{get:()=>" << profile.screen_height << "});";
    ss << "Object.defineProperty(screen,'availWidth',{get:()=>" << profile.screen_avail_width << "});";
    ss << "Object.defineProperty(screen,'availHeight',{get:()=>" << profile.screen_avail_height << "});";
    return ss.str();
}

std::string InjectionEngine::BuildPerformanceOverrides() const {
    return R"(performance.now=function(){const orig=performance.now.bind(performance);return orig()+(Math.random()-0.5)*20;};)";
}

std::string InjectionEngine::BuildDeviceOverrides(const FingerprintProfile& profile) const {
    std::stringstream ss;
    ss << "navigator.maxTouchPoints=" << (profile.touchscreen_support ? 5 : 0) << ";";
    ss << "navigator.mouseSupport=" << profile.mouse_support << ";";
    return ss.str();
}

std::string InjectionEngine::BuildStorageOverrides(const FingerprintProfile& profile) const {
    std::stringstream ss;
    ss << "localStorage.enabled=" << (profile.localStorage_enabled ? "true" : "false") << ";";
    ss << "sessionStorage.enabled=" << (profile.sessionStorage_enabled ? "true" : "false") << ";";
    ss << "indexedDB.enabled=" << (profile.indexeddb_enabled ? "true" : "false") << ";";
    return ss.str();
}

std::string InjectionEngine::BuildHttpHeaderOverrides(const FingerprintProfile& profile) const {
    std::stringstream ss;
    ss << "navigator.languages=['" << profile.accept_language << "'];";
    ss << "navigator.connection.effectiveType='" << (profile.effective_type_4g ? "4g" : "3g") << "';";
    return ss.str();
}

}  // namespace amnesia
