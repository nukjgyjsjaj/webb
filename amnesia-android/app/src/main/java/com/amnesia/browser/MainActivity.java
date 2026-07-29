package com.amnesia.browser;

import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.LinearLayout;
import android.widget.TextView;
import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.Lifecycle;
import androidx.lifecycle.LifecycleObserver;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.OnLifecycleEvent;
import com.google.android.material.appbar.MaterialToolbar;

public class MainActivity extends AppCompatActivity implements LifecycleObserver {

    private static final String TAG = "AmnesiaBrowser";
    private AmnesiaWebView webView;
    private EditText urlInput;
    private LinearLayout rootLayout;
    private boolean isInitialized = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initializeToolbar();
        initializeWebView();
        initializeUrlInput();
        initializeEngine();
    }

    private void initializeToolbar() {
        MaterialToolbar toolbar = findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);
    }

    private void initializeWebView() {
        webView = findViewById(R.id.webview);
        webView.clearAllData();
    }

    private void initializeUrlInput() {
        urlInput = findViewById(R.id.url_input);
        ImageButton goButton = findViewById(R.id.go_button);
        ImageButton refreshButton = findViewById(R.id.refresh_button);

        goButton.setOnClickListener(v -> navigateToUrl());
        refreshButton.setOnClickListener(v -> refreshPage());

        urlInput.setOnEditorActionListener((TextView v, int actionId, KeyEvent event) -> {
            if (actionId == EditorInfo.IME_ACTION_GO ||
                (event != null && event.getKeyCode() == KeyEvent.KEYCODE_ENTER)) {
                navigateToUrl();
                return true;
            }
            return false;
        });
    }

    private void navigateToUrl() {
        String url = urlInput.getText().toString().trim();
        if (url.isEmpty()) return;

        if (!url.startsWith("http://") && !url.startsWith("https://")) {
            url = "https://" + url;
        }

        urlInput.setText(url);
        webView.loadUrlSafe(url);
        AmnesiaEngine.navigate(url);
    }

    private void refreshPage() {
        webView.reload();
        AmnesiaEngine.reinitializeWithNewProfile();
    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK && webView.canGoBack()) {
            webView.goBack();
            return true;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (webView != null) {
            webView.clearAllData();
            webView.destroy();
        }
        AmnesiaEngine.shutdown();
    }

    @Override
    public void onBackPressed() {
        if (webView != null && webView.canGoBack()) {
            webView.goBack();
        } else {
            super.onBackPressed();
        }
    }
}
            });
        }).start();
    }
