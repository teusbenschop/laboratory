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
using System.IO;
using System.IO.IsolatedStorage;
using System.Windows;


namespace Bibledit
{

  public partial class Form1 : Form
  {

    string windowstate = "windowstate.txt";
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
      // Refresh restore bounds from previous window opening
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForAssembly();
      try
      {
        using (IsolatedStorageFileStream stream = new IsolatedStorageFileStream(this.windowstate, FileMode.Open, storage))
        using (StreamReader reader = new StreamReader(stream))
        {

          // Read restore bounds value from file
          string value;
          value = reader.ReadLine();
          if (value == "Maximized") {
            this.WindowState = FormWindowState.Maximized;
          } else {
            value = reader.ReadLine();
            if (value != "") this.Left = Int32.Parse(value);
            value = reader.ReadLine();
            if (value != "") this.Top = Int32.Parse(value);
            value = reader.ReadLine();
            if (value != "") this.Width = Int32.Parse(value);
            value = reader.ReadLine();
            if (value != "") this.Height = Int32.Parse(value);
          }
        }
      }
      catch (FileNotFoundException)
      {
        // Handle when file is not found in isolated storage, which is when:
        // * This is the first application session.
        // * The file has been deleted.
      }

    }




    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
      // Save window state for the next time this window is opened.
      IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForAssembly();
      using (IsolatedStorageFileStream stream = new IsolatedStorageFileStream(this.windowstate, FileMode.Create, storage))
      using (StreamWriter writer = new StreamWriter(stream))
      {
        // Write window state to file.
        writer.WriteLine(this.WindowState.ToString());
        writer.WriteLine(this.Location.X.ToString ());
        writer.WriteLine(this.Location.Y.ToString());
        writer.WriteLine(this.Size.Width.ToString());
        writer.WriteLine(this.Size.Height.ToString());
      }
    }

    private void webBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
    {

    }
  }
}
