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
import java.util.Timer
import kotlin.concurrent.schedule

class MainActivity : AppCompatActivity() {

    lateinit var webView: WebView
    val timer: Timer = Timer()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.single_view)
        WebView.setWebContentsDebuggingEnabled(true)
        webView = findViewById(R.id.singleview)
        applySettingsToWebView(webView)
        webView.loadUrl("https://bibledit.org:8091/exports/Sample/usfm/full")
        this.timer.schedule(1000L, 1000L) {
            onRepeatingTimeout()
        }
    }

    fun onRepeatingTimeout ()
    {
        runOnUiThread {

        }
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
        
        webView.setDownloadListener { url, userAgent, contentDisposition, mimeType, contentLength ->

            // Extract the filename from the Content-Disposition header or from the URL.
            val filename = URLDecoder.decode(getFilename(contentDisposition, url), "UTF-8")

            Log.i("Download", "Download $filename from $url as $userAgent")

            val request = DownloadManager.Request(url.toUri())
            request.apply {
                setTitle(filename)
                setDescription("Downloading file...")
                setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED)
                setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, filename)
                setMimeType(mimeType)
            }

            val downloadManager = getSystemService(DOWNLOAD_SERVICE) as DownloadManager
            downloadManager.enqueue(request)

            Toast.makeText(this, "Downloading $filename $contentLength bytes to the Downloads directory", Toast.LENGTH_LONG).show()
        }
    }

    private fun getFilename (contentDisposition : String?, url: String) : String {
        return if (contentDisposition != null && contentDisposition.contains("filename")) {
            contentDisposition.substringAfter("filename=").replace("\"", "")
        } else {
            url.substringAfterLast("/")
        }
    }
}