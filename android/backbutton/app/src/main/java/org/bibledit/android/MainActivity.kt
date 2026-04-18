package org.bibledit.android

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import android.webkit.WebView
import androidx.activity.OnBackPressedCallback
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {

    lateinit var webView: WebView

    @SuppressLint("SetJavaScriptEnabled")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.web_view)
        WebView.setWebContentsDebuggingEnabled(true)
        webView = findViewById(R.id.web_view)
        webView.settings.javaScriptEnabled = true
        webView.settings.builtInZoomControls = false
        webView.settings.setSupportZoom(false)
        webView.settings.displayZoomControls = false
        webView.settings.domStorageEnabled = true
        MyWebViewClient().also { webView.webViewClient = it }
        webView.loadUrl("https://bibledit.org:8091")

        onBackPressedDispatcher.addCallback(
            this,
            object : OnBackPressedCallback(true) {
                override fun handleOnBackPressed() {
                    handleBackPress()
                }
            })
    }


    fun handleBackPress() {
        Log.i("Back", "on back pressed")

        // The Android back button navigates back in the web view.
        // This is the behavior people expect.
        if (webView.canGoBack()) {
            webView.goBack()
            return
        }

        // Otherwise defer to the default behavior.
        finish()
    }

}