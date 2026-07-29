package com.amnesia.browser;

import android.content.Context;
import android.util.AttributeSet;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
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
        settings.setDomStorageEnabled(true);
        settings.setDatabaseEnabled(true);
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
        settings.setMixedContentMode(WebSettings.MIXED_CONTENT_COMPATIBILITY_MODE);
        settings.setUserAgentString(AmnesiaEngine.getSpoofedUserAgent());

        setWebViewClient(new WebViewClient() {
            @Override
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                view.loadUrl(url);
                return true;
            }
        });
    }

    public void loadUrlSafe(String url) {
        if (url != null && !url.isEmpty()) {
            loadUrl(url);
        }
    }

    public void clearAllData() {
        clearHistory();
        clearCache(true);
        clearFormData();
        setWebViewClient(new WebViewClient());
    }
}
