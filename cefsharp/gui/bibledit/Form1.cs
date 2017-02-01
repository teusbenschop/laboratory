using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Net;
using CefSharp;
using CefSharp.WinForms;

namespace Bibledit
{

  public partial class Form1 : Form
  {

    public ChromiumWebBrowser browser;
    public void InitBrowser()
    {
      Cef.Initialize(new CefSettings());
      browser = new ChromiumWebBrowser("http://bibledit.org:8080");
      this.Controls.Add(browser);
      browser.Dock = DockStyle.Fill;
    }


    public Form1()
    {
      InitializeComponent();
      InitBrowser();
    }


    private void Form1_Load(object sender, EventArgs e)
    {
    }




    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
    }

    private void webBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
    {

    }
  }
}
