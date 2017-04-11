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
    
    private ValueCallback<Uri> myUploadMessage;
    private final static int FILECHOOSER_RESULTCODE = 1;
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        if (requestCode == FILECHOOSER_RESULTCODE) {
            if (null == myUploadMessage) return;
            Uri result = intent == null || resultCode != RESULT_OK ? null : intent.getData();
            myUploadMessage.onReceiveValue (result);
            myUploadMessage = null;
        }
    }
    
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
        webview.setWebChromeClient(new WebChromeClient() {
            // The undocumented method overrides.
            // The compiler fails if you try to put @Override here.
            // It needs three interfaces to handle the various versions of Android.
            public void openFileChooser(ValueCallback<Uri> uploadMsg) {
                myUploadMessage = uploadMsg;
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                MainActivity.this.startActivityForResult(Intent.createChooser(intent, "File Chooser"), FILECHOOSER_RESULTCODE);
            }
            public void openFileChooser( ValueCallback uploadMsg, String acceptType) {
                myUploadMessage = uploadMsg;
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                MainActivity.this.startActivityForResult(Intent.createChooser(intent, "File Browser"), FILECHOOSER_RESULTCODE);
            }
            public void openFileChooser(ValueCallback<Uri> uploadMsg, String acceptType, String capture) {
                myUploadMessage = uploadMsg;
                Intent intent = new Intent(Intent.ACTION_GET_CONTENT);
                intent.addCategory(Intent.CATEGORY_OPENABLE);
                intent.setType("*/*");
                MainActivity.this.startActivityForResult (Intent.createChooser (intent, "File Chooser"), MainActivity.FILECHOOSER_RESULTCODE);
            }
        });
    }

}


