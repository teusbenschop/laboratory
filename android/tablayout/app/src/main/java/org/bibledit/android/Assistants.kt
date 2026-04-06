package org.bibledit.android

import android.webkit.WebView

// Apply settings to the passed WebView.
// Kotlin always use pass-by-value.
// When passing objects or non-primitive types, the function copies the reference, simulating pass-by-reference.
// Changes inside the method affect the external object due to the shared reference.
fun applySettingsToWebView (webView: WebView?)
{
    webView!!.getSettings().setJavaScriptEnabled(true)
    webView!!.getSettings().setBuiltInZoomControls(false)
    webView!!.getSettings().setSupportZoom(false)
    webView!!.getSettings().setDisplayZoomControls(false)

    webView!!.getSettings().setDomStorageEnabled(true)

    // Without this line the URL will open in an external browser.
    // With this line, the URL will open within the app.
    MyWebViewClient().also { webView!!.webViewClient = it }
}
