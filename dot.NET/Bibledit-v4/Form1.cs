using Microsoft.Web.WebView2.Core;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Bibledit_v4
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            // https://learn.microsoft.com/en-us/microsoft-edge/webview2/get-started/winforms
            webView.NavigationStarting += NavigationStarting;
            webView.NavigationCompleted += NavigationCompleted;
            webView.SourceChanged += SourceChanged;
            InitializeAsync();
        }


        void NavigationStarting(object sender, CoreWebView2NavigationStartingEventArgs args)
        {
            String uri = args.Uri;
            Console.WriteLine(uri);
        }

        
        
        void NavigationCompleted(object sender, CoreWebView2NavigationCompletedEventArgs args)
        {
            Console.WriteLine("NavigationCompleted");
        }


        void SourceChanged(object sender, CoreWebView2SourceChangedEventArgs args)
        {
            Console.WriteLine("SourceChanged");
        }


        async void InitializeAsync()
        {
            // Await EnsureCoreWebView2Async, because the initialization of CoreWebView2 is asynchronous.
            await webView.EnsureCoreWebView2Async(null);
            Console.WriteLine("Core webview is initialized");
            webView.CoreWebView2.Navigate("https://bibledit.org:8091");
        }

    }
}
