package org.bibledit.android

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.webkit.WebView
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.appcompat.app.AppCompatActivity

class Tab1 : Fragment() {

    lateinit var fragment: View
    lateinit var webview: WebView

    // This is called once in the app's lifetime.
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    // This is called each time the tab is (assumedly about to be) displayed.
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        // If the widget has not been initialized, do so once.
        // Goal: The WebView does not get reloaded each time the user goes to the tab.
        if (!this::fragment.isInitialized) {
            fragment = inflater.inflate(R.layout.fragment_tab1, container, false)
            webview = fragment.findViewById<WebView>(R.id.tab1webview)
            applySettingsToWebView(webview)
            webview?.loadUrl("https://bibledit.org:8091")
        }

        return fragment
    }

}