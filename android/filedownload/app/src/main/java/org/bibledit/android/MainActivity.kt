package org.bibledit.android

import android.Manifest
import android.annotation.SuppressLint
import android.content.Intent
import android.content.pm.PackageManager
import android.content.res.Configuration
import android.net.Uri
import android.os.Bundle
import android.util.Log
import android.view.ActionMode
import android.view.Menu
import android.view.View
import android.view.WindowManager
import android.view.inputmethod.InputMethodManager
import android.webkit.WebView
import androidx.appcompat.app.AppCompatActivity
import androidx.core.app.ActivityCompat
import androidx.core.app.ActivityCompat.requestPermissions
import androidx.core.content.edit
import com.google.android.material.tabs.TabLayout
import com.google.android.material.tabs.TabLayout.OnTabSelectedListener
import org.json.JSONArray
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream
import java.util.Timer
import kotlin.concurrent.schedule




// The activity's data is at /data/data/org.bibledit.android.
// It writes files to subfolder "files".

class MainActivity : AppCompatActivity() {

    var webView: WebView? = null
    val timer: Timer = Timer()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.single_view)
        WebView.setWebContentsDebuggingEnabled(true)
        this.timer.schedule(1000L, 1000L) {
            onRepeatingTimeout()
        }

        webView = findViewById<WebView>(R.id.singleview)
        applySettingsToWebView(webView)
        webView?.loadUrl("https://bibledit.org:8091")
    }



    fun onRepeatingTimeout ()
    {
//                runOnUiThread {
//                    getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
//                }
    }

    private fun applySettingsToWebView (webView: WebView?)
    {
        @SuppressLint("SetJavaScriptEnabled")
        webView!!.settings.setJavaScriptEnabled(true)
        webView!!.settings.setBuiltInZoomControls(false)
        webView!!.settings.setSupportZoom(false)
        webView!!.settings.setDisplayZoomControls(false)
        webView!!.settings.setDomStorageEnabled(true)
        MyWebViewClient().also { webView!!.webViewClient = it }
    }







}