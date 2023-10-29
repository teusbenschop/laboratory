using System;
using System.Diagnostics;
using System.Windows.Forms;
using CefSharp;
using CefSharp.WinForms;
using System.IO;
using System.IO.IsolatedStorage;
using System.Runtime.InteropServices;
using System.Timers;
using System.Net.Sockets;
using Microsoft.Win32;
using System.Text.RegularExpressions;
using System.Windows.Threading;
using CefSharp.Handler;

namespace Bibledit
{

    public partial class Form1 : Form
    {

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr SetWindowsHookEx(int idHook, LowLevelKeyboardProc lpfn, IntPtr hMod, uint dwThreadId);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool UnhookWindowsHookEx(IntPtr hhk);

        [DllImport("user32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr CallNextHookEx(IntPtr hhk, int nCode, IntPtr wParam, IntPtr lParam);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        private static extern IntPtr GetModuleHandle(string lpModuleName);


        private static IntPtr MyHookID = IntPtr.Zero;
        private const int WH_KEYBOARD_LL = 13;
        private const int WM_KEYDOWN = 0x0100;
        private const int WM_KEYUP = 0x0101;
        private static LowLevelKeyboardProc MyKeyboardProcessor = HookCallback;
        private static Boolean HasFocus = false;
        private static Boolean ControlPressed = false;
        private static Boolean SearchDialogOpen = false;


        string windowstate = "windowstate.txt";
        public static ChromiumWebBrowser browser = null;


        private delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, IntPtr lParam);


        private DispatcherTimer dispatcherTimer = null;
        String portNumber = String.Empty;


        private static Boolean PrintDialogOpen = false;


        private static IntPtr SetHook(LowLevelKeyboardProc proc)
        {
            using (Process curProcess = Process.GetCurrentProcess())
            using (ProcessModule curModule = curProcess.MainModule)
            {
                return SetWindowsHookEx(WH_KEYBOARD_LL, proc, GetModuleHandle(curModule.ModuleName), 0);
            }
        }


        private static IntPtr HookCallback(int nCode, IntPtr wParam, IntPtr lParam)
        {
            if (HasFocus)
            {
                if (nCode >= 0)
                {
                    int vkCode = Marshal.ReadInt32(lParam);
                    Keys keys = (Keys)vkCode;
                    String key = keys.ToString();
                    if (wParam == (IntPtr)WM_KEYDOWN)
                    {
                        if (key.Contains("Control")) ControlPressed = true;
                    }
                    if (wParam == (IntPtr)WM_KEYUP)
                    {
                        if (key.Contains("Control")) ControlPressed = false;
                    }
                    if (ControlPressed && key.Equals("F") && (wParam == (IntPtr)WM_KEYDOWN))
                    {
                        SearchWebkit();
                    }
                    if (ControlPressed && key.Equals("P") && (wParam == (IntPtr)WM_KEYDOWN))
                    {
                        PrintWebkit();
                    }
                }
            }
            return CallNextHookEx(MyHookID, nCode, wParam, lParam);
        }


        public void InitBrowser()
        {
            Debug.WriteLine("init");
            Cef.Initialize(new CefSettings());
            browser = new ChromiumWebBrowser("https://bibledit.org:8091");
            browser.DownloadHandler = new DownloadHandler();
            Controls.Add(browser);
            browser.Dock = DockStyle.Fill;
        }


        public Form1()
        {
            InitializeComponent();
            InitBrowser();
            MyHookID = SetHook(MyKeyboardProcessor);
            dispatcherTimer = new DispatcherTimer();
            dispatcherTimer.Tick += new EventHandler(GuiUpdaterTick);
            dispatcherTimer.Interval = new TimeSpan(0, 0, 0, 0, 50);
            dispatcherTimer.Start();
        }


        ~Form1()
        {
            UnhookWindowsHookEx(MyHookID);
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            // Set screen size and state as was saved by the previous session.
            IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForAssembly();
            try
            {
                using (IsolatedStorageFileStream stream = new IsolatedStorageFileStream(windowstate, FileMode.Open, storage))
                using (StreamReader reader = new StreamReader(stream))
                {

                    // Read restore bounds value from file.
                    string value;
                    value = reader.ReadLine();
                    if (value == "Maximized")
                    {
                        WindowState = FormWindowState.Maximized;
                    }
                    else
                    {
                        int screen_left = Screen.FromControl(this).Bounds.Left;
                        int screen_width = Screen.FromControl(this).Bounds.Width;
                        int screen_top = Screen.FromControl(this).Bounds.Top;
                        int screen_height = Screen.FromControl(this).Bounds.Height;
                        int form_left = screen_left;
                        int form_width = screen_width;
                        int form_top = screen_top;
                        int form_height = screen_height;
                        bool reset_screen_sizes = false;
                        value = reader.ReadLine();
                        if (value != "") form_left = Int32.Parse(value);
                        value = reader.ReadLine();
                        if (value != "") form_top = Int32.Parse(value);
                        value = reader.ReadLine();
                        if (value != "") form_width = Int32.Parse(value);
                        value = reader.ReadLine();
                        if (value != "") form_height = Int32.Parse(value);
                        // Fix negative or bad values. Allow for a small error margin.
                        if (form_left < screen_left - 25) reset_screen_sizes = true;
                        if (form_width + form_left > screen_left + screen_width + 50) reset_screen_sizes = true;
                        if (form_top < screen_top - 25) reset_screen_sizes = true;
                        if (form_height + form_top > screen_height + screen_top + 50) reset_screen_sizes = true;
                        if (form_height < 300) reset_screen_sizes = true;
                        if (form_width < 400) reset_screen_sizes = true;
                        if (!reset_screen_sizes)
                        {
                            Left = form_left;
                            Top = form_top;
                            Width = form_width;
                            Height = form_height;
                        }
                    }
                }
            }
            catch (FileNotFoundException)
            {
                // Handle case when file is not found in isolated storage.
                // This happens on starting the app for the first time.
                // Or when the file is no longer available.
            }
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                // Save window state for the next time this window is opened.
                IsolatedStorageFile storage = IsolatedStorageFile.GetUserStoreForAssembly();
                using (IsolatedStorageFileStream stream = new IsolatedStorageFileStream(windowstate, FileMode.Create, storage))
                using (StreamWriter writer = new StreamWriter(stream))
                {
                    // Write window state to file.
                    writer.WriteLine(WindowState.ToString());
                    writer.WriteLine(Location.X.ToString());
                    writer.WriteLine(Location.Y.ToString());
                    writer.WriteLine(Size.Width.ToString());
                    writer.WriteLine(Size.Height.ToString());
                }
            }
            catch (FileNotFoundException)
            {
                // Handle case when file cannot be stored in isolated storage.
                // This has happened in the case reported below.
                // https://github.com/bibledit/cloud/issues/266
            }
        }


        private void Form1_Activated(object sender, EventArgs e)
        {
            HasFocus = true;
        }


        private void Form1_Deactivate(object sender, EventArgs e)
        {
            HasFocus = false;
        }


        private static void SearchWebkit()
        {
            if (SearchDialogOpen) return;
            SearchDialogOpen = true;

            System.Drawing.Size size = new System.Drawing.Size(200, 70);
            Form inputBox = new Form();

            inputBox.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            inputBox.ClientSize = size;
            inputBox.Text = "Search";

            System.Windows.Forms.TextBox textBox = new TextBox();
            textBox.Size = new System.Drawing.Size(size.Width - 10, 23);
            textBox.Location = new System.Drawing.Point(5, 5);
            inputBox.Controls.Add(textBox);

            Button okButton = new Button();
            okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
            okButton.Size = new System.Drawing.Size(75, 23);
            okButton.Text = "&OK";
            okButton.Location = new System.Drawing.Point(size.Width - 80 - 80, 39);
            inputBox.Controls.Add(okButton);

            Button cancelButton = new Button();
            cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            cancelButton.Size = new System.Drawing.Size(75, 23);
            cancelButton.Text = "&Cancel";
            cancelButton.Location = new System.Drawing.Point(size.Width - 80, 39);
            inputBox.Controls.Add(cancelButton);

            inputBox.FormBorderStyle = FormBorderStyle.FixedDialog;
            inputBox.MinimizeBox = false;
            inputBox.MaximizeBox = false;

            inputBox.AcceptButton = okButton;
            inputBox.CancelButton = cancelButton;

            inputBox.StartPosition = FormStartPosition.CenterParent;
            DialogResult result = inputBox.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.OK)
            {
                string search = textBox.Text;
                // If the users enters an empty string, any markup is supposed to be removed from the webview.
                // This is done by searching for something that is not likely to be found.
                if (search.Length == 0) search = "b.i.b.l.e.d.i.t";
                browser.GetBrowserHost().Find(search, true, false, true);
            }

            SearchDialogOpen = false;
        }


        private void OnTimedEvent(Object source, ElapsedEventArgs e)
        {
        }


        private void OnFocusedReferenceTimedEvent(Object source, ElapsedEventArgs e)
        {
        }


        private static void PrintWebkit()
        {
            if (PrintDialogOpen) return;
            PrintDialogOpen = true;
            browser.Print();
            PrintDialogOpen = false;
        }


        private void GuiUpdaterTick(object sender, EventArgs e)
        {
            if (browser != null) return;
            if (String.IsNullOrEmpty(portNumber)) return;
            InitBrowser();
        }


    }
}
