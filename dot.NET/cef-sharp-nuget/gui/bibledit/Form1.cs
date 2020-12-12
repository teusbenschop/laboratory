using System;
using System.Diagnostics;
using System.Windows.Forms;
using System.IO;
using System.IO.IsolatedStorage;
using System.Runtime.InteropServices;
using System.Timers;
using System.Net.Sockets;
using Microsoft.Win32;
using CefSharp.WinForms;


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


        //public static WebBrowser browser;
        public static ChromiumWebBrowser browser;


        private delegate IntPtr LowLevelKeyboardProc(int nCode, IntPtr wParam, IntPtr lParam);


        private System.Timers.Timer timer;


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
            //Cef.Initialize(new CefSettings());
            //browser = new WebBrowser();
            browser = new ChromiumWebBrowser("http://localhost:9876");
            //browser.DownloadHandler = new DownloadHandler();
            Controls.Add(browser);
            browser.Dock = DockStyle.Fill;
            
            //browser.Navigate("https://bibledit.org");
        }


        public Form1()
        {
            InitializeComponent();
            InitBrowser();
            MyHookID = SetHook(MyKeyboardProcessor);

            // Create a timer with a one-second interval.
            timer = new System.Timers.Timer(1000);
            // Hook up the Elapsed event for the timer. 
            timer.Elapsed += onTimerElapsed;
            timer.AutoReset = false;
            timer.Start();
        }


        ~Form1()
        {
            UnhookWindowsHookEx(MyHookID);
            timer.Stop();
            timer.Dispose();
        }


        private void Form1_Load(object sender, EventArgs e)
        {
        }


        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
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
                //WebBrowserExtensions.Find(browser, 1, search, true, false, false);
            }

            SearchDialogOpen = false;
        }


        private void onTimerElapsed(Object source, ElapsedEventArgs e)
        {
            timer.Stop();
            timer.Start();
        }


        private static void PrintWebkit()
        {
            if (PrintDialogOpen) return;
            PrintDialogOpen = true;
            //browser.Print();
            PrintDialogOpen = false;
        }


    }
}
