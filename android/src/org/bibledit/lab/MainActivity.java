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


public class MainActivity extends Activity
{
    WebView webview;
    
    @Override
    public void onCreate (Bundle savedInstanceState)
    {
        super.onCreate (savedInstanceState);
        Log.d ("Bibledit", "onCreate");
        webview = new WebView (this);
        setContentView (webview);
        webview.getSettings().setJavaScriptEnabled (true);
        webview.getSettings().setBuiltInZoomControls (true);
        webview.getSettings().setSupportZoom (true);
        webview.setWebViewClient(new WebViewClient());
        webview.loadUrl ("http://bibledit.org:8080");
    }

}


