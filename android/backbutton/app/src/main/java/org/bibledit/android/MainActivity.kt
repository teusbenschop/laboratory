package org.bibledit.android

import android.annotation.SuppressLint
import android.app.DownloadManager
import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.webkit.WebView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.core.net.toUri
import java.net.URLDecoder

class MainActivity : AppCompatActivity() {

    lateinit var webView: WebView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.web_view)
        WebView.setWebContentsDebuggingEnabled(true)
        webView = findViewById(R.id.web_view)
        applySettingsToWebView(webView)
        webView.loadUrl("https://bibledit.org:8091")
    }


    private fun applySettingsToWebView (webView: WebView)
    {
        @SuppressLint("SetJavaScriptEnabled")
        webView.settings.javaScriptEnabled = true
        webView.settings.builtInZoomControls = false
        webView.settings.setSupportZoom(false)
        webView.settings.displayZoomControls = false
        webView.settings.domStorageEnabled = true
        MyWebViewClient().also { webView.webViewClient = it }
    }

}