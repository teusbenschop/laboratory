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


class MainActivity : AppCompatActivity() {

    var webview: WebView? = null
    var tabhost: TabHost? = null
    var tablayout : TabLayout? = null
    var viewpager: ViewPager? = null
    lateinit var timer: Timer
    var viewstate : Int = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.splash_screen)
        WebView.setWebContentsDebuggingEnabled(true)
        startTabLayoutView()
//        timer = Timer()
//        timer.schedule(1000L, 3000L) {
//            onRepeatingTimeout()
//        }
    }

    fun onRepeatingTimeout ()
    {
        viewstate += 1
        if (viewstate > 3) {
            viewstate = 0
        }

        when (viewstate) {
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
        tablayout = findViewById(R.id.tabLayout)
        viewpager = findViewById(R.id.viewPager)
        tablayout!!.addTab(tablayout!!.newTab().setText("Tab 1"))
        tablayout!!.addTab(tablayout!!.newTab().setText("Tab 2"))
        tablayout!!.addTab(tablayout!!.newTab().setText("Tab 3"))
        tablayout!!.tabGravity = TabLayout.GRAVITY_FILL
        val adapter = TabAdapter(this, supportFragmentManager,
            tablayout!!.tabCount)
        viewpager!!.adapter = adapter
        viewpager!!.addOnPageChangeListener(TabLayoutOnPageChangeListener(tablayout))
        tablayout!!.addOnTabSelectedListener(object : OnTabSelectedListener {
            override fun onTabSelected(tab: TabLayout.Tab) {
                viewpager!!.currentItem = tab.position
                println("selected tab " + tab.position.toString())
            }
            override fun onTabUnselected(tab: TabLayout.Tab) {
//                println("unselected tab " + tab.position.toString())
            }
            override fun onTabReselected(tab: TabLayout.Tab) {
//                println("reselected tab " + tab.position.toString())
            }
        })
    }

    private fun nullAllWidgets() {
        webview = null
        tabhost = null
        tablayout = null
        viewpager = null
    }

}