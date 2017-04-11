package org.bibledit.lab;


import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.preference.PreferenceManager;
import android.content.SharedPreferences;
import java.io.FileOutputStream;
import android.content.Context;
import android.util.Log;
import java.io.OutputStreamWriter;
import android.content.res.AssetManager;
import java.io.IOException;
import java.lang.String;
import java.io.InputStream;
import java.io.File;
import java.io.OutputStream;
import android.os.Environment;
import android.content.Intent;
import android.view.View;
import android.app.Notification;
import android.app.PendingIntent;
import android.app.NotificationManager;
import android.webkit.WebView;
import java.util.TimeZone;
import java.util.GregorianCalendar;
import java.util.Calendar;
import android.content.res.AssetManager;
import android.webkit.WebViewClient;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.os.Handler;
import android.net.Uri;
import java.util.Timer;
import java.util.TimerTask;
import android.os.Process;
import android.view.WindowManager;
import android.webkit.DownloadListener;
import android.app.DownloadManager;
import android.widget.Toast;


public class MainActivity extends Activity
{
    
    WebView webview;
    
    
    // Function is called when the app gets launched.
    @Override
    public void onCreate (Bundle savedInstanceState)
    {
        super.onCreate (savedInstanceState);
        // Log.d ("Bibledit", "onCreate");
        
        webview = new WebView (this);
        setContentView (webview);
        webview.getSettings().setJavaScriptEnabled (true);
        webview.getSettings().setBuiltInZoomControls (true);
        webview.getSettings().setSupportZoom (true);
        webview.setWebViewClient(new WebViewClient());
        webview.loadUrl ("http://bibledit.org:8080");
        webview.setDownloadListener(new DownloadListener() {
            @Override
            public void onDownloadStart (String url, String userAgent, String contentDisposition, String mimetype,
                                        long contentLength) {
                DownloadManager.Request request = new DownloadManager.Request(Uri.parse(url));
                request.allowScanningByMediaScanner();
                // Notify client once download is completed.
                request.setNotificationVisibility(DownloadManager.Request.VISIBILITY_VISIBLE_NOTIFY_COMPLETED);
                Uri uri = Uri.parse (url);
                String filename = uri.getLastPathSegment ();
                request.setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, filename);
                DownloadManager dm = (DownloadManager) getSystemService(DOWNLOAD_SERVICE);
                dm.enqueue(request);
                // Notification that the file is being downloaded.
                Toast.makeText(getApplicationContext(), "Downloading file", Toast.LENGTH_LONG).show();
            }
        });
    }
    
    
    @Override
    public boolean onCreateOptionsMenu (Menu menu)
    {
        return false;
    }
    
    
    // Function is called when the user starts the app.
    @Override
    protected void onStart ()
    {
        // Log.d ("Bibledit", "onStart");
        super.onStart();
    }
    
    
    // Function is called when the user returns to the activity.
    @Override
    protected void onRestart ()
    {
        // Log.d ("Bibledit", "onRestart");
        super.onRestart();
    }
    
    
    // Function is called when the app is moved to the foreground again.
    @Override
    public void onResume ()
    {
        // Log.d ("Bibledit", "onResume");
        super.onResume();
    }
    
    
    // Function is called when the app is obscured.
    @Override
    public void onPause ()
    {
        // Log.d ("Bibledit", "onPause");
        super.onPause ();
    }
    
    
    // Function is called when the user completely leaves the activity.
    @Override
    protected void onStop ()
    {
        // Log.d ("Bibledit", "onStop");
        super.onStop();
    }
    
    
    // Function is called when the app gets completely destroyed.
    @Override
    public void onDestroy ()
    {
        // Log.d ("Bibledit", "onDestroy");
        super.onDestroy ();
    }
    
    
    @Override
    public void onBackPressed() {
        // The Android back button navigates back in the web view.
        // That is the behaviour people expect.
        if (webview.canGoBack()) {
            webview.goBack();
            return;
        }
        
        // Otherwise defer to system default behavior.
        super.onBackPressed();
    }
    
}
