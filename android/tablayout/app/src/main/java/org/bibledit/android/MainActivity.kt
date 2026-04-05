package org.bibledit.android

import android.os.Bundle
import android.webkit.WebView
import android.widget.TabHost
import android.widget.TabHost.OnTabChangeListener
import android.widget.TabHost.TabContentFactory
import android.widget.TabHost.TabSpec
import androidx.appcompat.app.AppCompatActivity
import androidx.constraintlayout.widget.ConstraintLayout
import androidx.viewpager.widget.ViewPager
import com.google.android.material.tabs.TabLayout
import com.google.android.material.tabs.TabLayout.OnTabSelectedListener
import com.google.android.material.tabs.TabLayout.TabLayoutOnPageChangeListener
import java.util.Timer
import kotlin.concurrent.schedule


class MainActivity : AppCompatActivity() {

    var webview: WebView? = null
    var tabhost: TabHost? = null
    var tabLayout : TabLayout? = null
    var viewPager: ViewPager? = null
    lateinit var timer: Timer
    var viewState : Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.splash_screen)
        WebView.setWebContentsDebuggingEnabled(true)
        timer = Timer()
        timer.schedule(1000L, 3000L) {
            onRepeatingTimeout()
        }
    }

    fun onRepeatingTimeout ()
    {
        viewState += 1
        if (viewState > 3) {
            viewState = 0
        }

        when (viewState) {
            0 -> {
                runOnUiThread {
                    setContentView(R.layout.splash_screen)
                }
            }
            1 -> {
                runOnUiThread {
                    startSingleView()
                }
            }
            2 -> {
                runOnUiThread {
                    startTabHostView()
                }
            }
            3 -> {
                runOnUiThread {
                    startTabLayoutView()
                }
            }
            else -> {

            }
        }
    }


    private fun startSingleView()
    {
        nullAllWidgets()
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
        webView!!.getSettings().setJavaScriptEnabled(true)
        webView!!.getSettings().setBuiltInZoomControls(false)
        webView!!.getSettings().setSupportZoom(false)
        webView!!.getSettings().setDisplayZoomControls(false)

        webView!!.getSettings().setDomStorageEnabled(true)

        // Without this line the URL will open in an external browser.
        // With this line, the URL will open within the app.
        MyWebViewClient().also { webView!!.webViewClient = it }
    }


    private fun startTabHostView() {
        nullAllWidgets()

        setContentView(R.layout.tabhost_view)

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

    private fun startTabLayoutView()
    {
        nullAllWidgets()

        setContentView(R.layout.tablayout_view)
        tabLayout = findViewById(R.id.tabLayout)
        viewPager = findViewById(R.id.viewPager)
        tabLayout!!.addTab(tabLayout!!.newTab().setText("Tab 1"))
        tabLayout!!.addTab(tabLayout!!.newTab().setText("Tab 2"))
        tabLayout!!.addTab(tabLayout!!.newTab().setText("Tab 3"))
        tabLayout!!.tabGravity = TabLayout.GRAVITY_FILL
        val adapter = MyAdapter(this, supportFragmentManager,
            tabLayout!!.tabCount)
        viewPager!!.adapter = adapter
        viewPager!!.addOnPageChangeListener(TabLayoutOnPageChangeListener(tabLayout))
        tabLayout!!.addOnTabSelectedListener(object : OnTabSelectedListener {
            override fun onTabSelected(tab: TabLayout.Tab) {
                viewPager!!.currentItem = tab.position
            }
            override fun onTabUnselected(tab: TabLayout.Tab) {}
            override fun onTabReselected(tab: TabLayout.Tab) {}
        })



    }

    private fun nullAllWidgets() {
        webview = null
        tabhost = null
        tabLayout = null
        viewPager = null
    }

}