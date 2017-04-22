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
import android.webkit.ValueCallback;
import android.webkit.WebChromeClient;
import android.graphics.Bitmap;
import android.widget.TabHost;


public class MainActivity extends Activity
{
    WebView webview;
    TabHost tabhost;
    Timer timer;
    TimerTask timerTask;

    
    @Override
    public void onCreate (Bundle savedInstanceState)
    {
        super.onCreate (savedInstanceState);
        Log.d ("Bibledit", "onCreate");
        setWebView ();
        //startTimer ();
    }

    
    private void startTimer ()
    {
        stopTimer ();
        timer = new Timer();
        initializeTimerTask();
        timer.schedule (timerTask, 2000);
    }
    
    
    private void stopTimer ()
    {
        if (timer != null) {
            timer.cancel();
            timer = null;
        }
    }

    
    private void initializeTimerTask() {
        timerTask = new TimerTask() {
            public void run() {
                Log.d ("Bibledit syncing", "one");
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        setWebView();
                    }
                });
                /*
                String syncState = IsSynchronizing ();
                if (syncState.equals ("true")) {
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                        }
                    });
                    // Log.d ("Bibledit", "keep screen on");
                }
                if (syncState.equals ("false")) {
                    if (syncState.equals (previousSyncState)) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                            }
                        });
                        // Log.d ("Bibledit", "do not keep screen on");
                    }
                }
                previousSyncState = syncState;
                String externalUrl = GetExternalUrl ();
                if (externalUrl != null && !externalUrl.isEmpty ()) {
                    Log.d ("Bibledit start Browser", externalUrl);
                    Intent browserIntent = new Intent (Intent.ACTION_VIEW, Uri.parse (externalUrl));
                    startActivity(browserIntent);
                }
                 */
                startTimer ();
            }
        };
    }
    
    
    private void setWebView ()
    {
        /*
        tabhost = null;
        tabhost = new TabHost (this);
        tabhost.setup ();
        */
        webview = null;
        webview = new WebView (this);
        webview.getSettings().setJavaScriptEnabled (true);
        webview.getSettings().setBuiltInZoomControls (true);
        webview.getSettings().setSupportZoom (true);
        webview.setWebViewClient(new WebViewClient());
        webview.loadUrl ("http://bibledit.org:8080");
        setContentView (webview);
    }


}


