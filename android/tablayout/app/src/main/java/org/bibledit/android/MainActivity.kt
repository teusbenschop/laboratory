package org.bibledit.android

import android.annotation.SuppressLint
import android.os.Bundle
import android.view.Menu
import android.webkit.WebView
import android.widget.TabHost
import android.widget.TabHost.OnTabChangeListener
import android.widget.TabHost.TabContentFactory
import android.widget.TabHost.TabSpec
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import java.util.Timer
import kotlin.concurrent.schedule


class MainActivity : AppCompatActivity() {

    var webview: WebView? = null
    var tabhost: TabHost? = null
    var timer: Timer? = null
    var previousTabsState: Boolean = false


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.splash_screen)
        WebView.setWebContentsDebuggingEnabled(true)
    }


    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Don't create the menu.
        return false
    }


    // Function is called when the user starts the app.
    override fun onStart() {
        super.onStart()
        startTimer()
    }


    // Function is called when the user returns to the activity.
    override fun onRestart() {
        super.onRestart()
        startTimer()
    }


    // Function is called when the app is moved to the foreground again.
    public override fun onResume() {
        super.onResume()
        startTimer()
    }


    // Function is called when the app is obscured.
    public override fun onPause() {
        super.onPause()
        stopTimer()
    }


    // Function is called when the user completely leaves the activity.
    override fun onStop() {
        super.onStop()
        stopTimer()
    }


    // Function is called when the app gets completely destroyed.
    public override fun onDestroy() {
        super.onDestroy()
        stopTimer()
    }

    fun onRepeatingTimeout ()
    {
        previousTabsState = !previousTabsState

        if (!previousTabsState) {
            runOnUiThread {
                startSingleView()
            }
        }

        if (previousTabsState) {
            runOnUiThread {
                startTabbedView()
            }
        }
    }


    private fun startSingleView()
    {
        tabhost = null
        setContentView(R.layout.single_view)
        webview = findViewById<WebView>(R.id.singleview)
        applySettingsToWebView(webview)
        webview?.loadUrl("https://bibledit.org:8091")
    }


    private fun getNewWebViewWithSettings () : WebView
    {
        val newWebview = WebView(this).apply {
            layoutParams = ConstraintLayout.LayoutParams(
                ConstraintLayout.LayoutParams.MATCH_PARENT,
                ConstraintLayout.LayoutParams.MATCH_PARENT
            ).apply {
                bottomToBottom = ConstraintLayout.LayoutParams.BOTTOM
                endToEnd = ConstraintLayout.LayoutParams.END
                startToStart = ConstraintLayout.LayoutParams.START
                topToTop = ConstraintLayout.LayoutParams.TOP
            }
        }

        @SuppressLint("SetJavaScriptEnabled")
        newWebview.getSettings().setJavaScriptEnabled(true)

        // No built-in zoom controls,
        // because these may cover clickable links,
        // which then can't be clicked anymore.
        // https://github.com/bibledit/cloud/issues/321
        newWebview.getSettings().setBuiltInZoomControls(false)
        newWebview.getSettings().setSupportZoom(false)
        newWebview.getSettings().setDisplayZoomControls(false)

        newWebview.getSettings().setDomStorageEnabled(true)

        // Without this line the URL will open in an external browser.
        // With this line, the URL will open within the app.
        //webview.webViewClient = MyWebViewClient()
        MyWebViewClient().also { newWebview.webViewClient = it }

        return newWebview
    }


    // Apply settings to the passed WebView.
    // Kotlin always use pass-by-value.
    // When passing objects or non-primitive types, the function copies the reference, simulating pass-by-reference.
    // Changes inside the method affect the external object due to the shared reference.
    private fun applySettingsToWebView (webView: WebView?)
    {
        @SuppressLint("SetJavaScriptEnabled")
        webview!!.getSettings().setJavaScriptEnabled(true)

        // No built-in zoom controls,
        // because these may cover clickable links,
        // which then can't be clicked anymore.
        // https://github.com/bibledit/cloud/issues/321
        webview!!.getSettings().setBuiltInZoomControls(false)
        webview!!.getSettings().setSupportZoom(false)
        webview!!.getSettings().setDisplayZoomControls(false)

        webview!!.getSettings().setDomStorageEnabled(true)

        // Without this line the URL will open in an external browser.
        // With this line, the URL will open within the app.
        MyWebViewClient().also { webview!!.webViewClient = it }
    }


    private fun startTabbedView() {
        webview = null

        setContentView(R.layout.tabbed_view)

        tabhost = findViewById (R.id.tabhost)
        tabhost!!.setup ()

        var tabspec: TabSpec?
        var factory: TabContentFactory?

        val length = 4
        var active = 0

        for (i in 0..<length) {
            val label = "Label $i"
            var url = ""
            if (i == 0)
                url = "editone/index"
            if (i == 1)
                url = "resource/index"
            if (i == 2)
                url = "notes/index"
            if (i == 3)
                url = "personalize/index"
            tabspec = tabhost!!.newTabSpec (label)
            tabspec.setIndicator (label)

            factory = TabHost.TabContentFactory () {
                val webview = getNewWebViewWithSettings()
                webview.loadUrl("https://bibledit.org:8091/" + url)
                return@TabContentFactory webview
            }

            tabspec.setContent(factory)
            tabhost!!.addTab (tabspec)

            if (url.contains("resource"))
                active = i
        }
        val tab: Int = active

        // It used to halve the height of the tabs on the screen.
        // The goal of that was to use less space on the screen,
        // leaving more space for the editing areas.
        // But a user made a remark that on his 8 inch tablet,
        // the tabbed menu was so small
        // that he often missed and the top Android status bar pulled down instead.
        // So it is better to not halve the height, but use another reduction factor.
        for (i in 0..<tabhost!!.getTabWidget().getChildCount()) {
            tabhost!!.getTabWidget().getChildAt(i).getLayoutParams().height =
                (tabhost!!.getTabWidget().getChildAt(i).getLayoutParams().height * 0.75).toInt()
        }

        tabhost!!.setCurrentTab (active)

        tabhost!!.setOnTabChangedListener(object : OnTabChangeListener {
            override fun onTabChanged(tabId: String) {
            }
        })
    }


    private fun startTimer()
    {
        stopTimer()
        timer = Timer()
        timer!!.schedule(1000L, 3000L) {
            onRepeatingTimeout()
        }
    }


    private fun stopTimer()
    {
        if (timer != null) {
            timer!!.cancel()
            timer = null
        }
    }

}