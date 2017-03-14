using System;
using System.Windows.Forms;
using System.IO;
using System.IO.IsolatedStorage;


namespace Bibledit
{

  public partial class Form1 : Form
  {


    public Form1()
    {
      InitializeComponent();
    }


    private void Form1_Load(object sender, EventArgs e)
    {
    }




    private void Form1_FormClosing(object sender, FormClosingEventArgs e)
    {
    }

    private void Form1_KeyDown_1(object sender, KeyEventArgs e)
    {
      if ((e.Control) && (e.KeyCode == Keys.F))
      {
        Console.WriteLine("Ctrl-F");

        System.Drawing.Size size = new System.Drawing.Size(200, 70);
        Form inputBox = new Form();

        inputBox.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        inputBox.ClientSize = size;
        inputBox.Text = "Search";

        System.Windows.Forms.TextBox textBox = new TextBox();
        textBox.Size = new System.Drawing.Size(size.Width - 10, 23);
        textBox.Location = new System.Drawing.Point(5, 5);
        textBox.Text = "search for, guys!";
        inputBox.Controls.Add(textBox);

        Button okButton = new Button();
        okButton.DialogResult = System.Windows.Forms.DialogResult.OK;
        okButton.Name = "okButton";
        okButton.Size = new System.Drawing.Size(75, 23);
        okButton.Text = "&OK";
        okButton.Location = new System.Drawing.Point(size.Width - 80 - 80, 39);
        inputBox.Controls.Add(okButton);

        Button cancelButton = new Button();
        cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        cancelButton.Name = "cancelButton";
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
        Console.WriteLine (textBox.Text);

      }
    }
  }
}
