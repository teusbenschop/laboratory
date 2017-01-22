#include <cstdlib>
#include <libgen.h>
#include <iostream>
#include <gtk/gtk.h>
#include <webkit2/webkit2.h>


using namespace std;


static void activate (GtkApplication *app);


int main (int argc, char *argv[])
{
  GtkApplication *application = gtk_application_new ("org.bibledit.linux", G_APPLICATION_FLAGS_NONE);
  
  g_signal_connect (application, "activate", G_CALLBACK (activate), NULL);
  
  // Get the executable path and base the document root on it.
  char *linkname = (char *) malloc (256);
  if (readlink ("/proc/self/exe", linkname, 256)) {};
  string webroot = dirname (linkname);
  free (linkname);
  
  gint status = g_application_run (G_APPLICATION (application), argc, argv);
  
  g_object_unref (application);
  
  return status;
}


gboolean on_key_press (GtkWidget *widget, GdkEvent *event, gpointer data)
{
  if (event->type == GDK_KEY_PRESS) {
    GdkEventKey * event_key = (GdkEventKey *) event;
    if ((event_key->keyval == GDK_KEY_z) || (event_key->keyval == GDK_KEY_Z)  ) {
      if (event_key->state & GDK_CONTROL_MASK) {
        const gchar *command;
        if (event_key->state & GDK_SHIFT_MASK) {
          command = WEBKIT_EDITING_COMMAND_REDO;
        } else {
          command = WEBKIT_EDITING_COMMAND_UNDO;
        }
        WebKitWebView * web_view = WEBKIT_WEB_VIEW (widget);
        webkit_web_view_execute_editing_command (web_view, command);
        return true;
      }
    }
  }
  (void) data;
  return false;
}


void on_signal_destroy (gpointer user_data)
{
  (void) user_data;
  gtk_main_quit ();
}


void activate (GtkApplication * application)
{
  GList *list = gtk_application_get_windows (application);
  
  if (list) {
    gtk_window_present (GTK_WINDOW (list->data));
    return;
  }
  
  // The top-level window.
  GtkWidget * window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "Bibledit");
  gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  gtk_window_set_application (GTK_WINDOW (window), application);
  
  // Prepare for program quit.
  g_signal_connect (window, "destroy", G_CALLBACK (on_signal_destroy), NULL);
  
  // Create a browser instance.
  WebKitWebView * webview = WEBKIT_WEB_VIEW (webkit_web_view_new ());
  
  // Signal handlers.
  g_signal_connect (webview, "key-press-event", G_CALLBACK (on_key_press), NULL);
  
  // Put the browser area into the main window.
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (webview));
  
  // Load a web page into the browser instance
  webkit_web_view_load_uri (webview, "http://bibledit.org:8080");
 
  // Ensure it will get mouse and keyboard events.
  gtk_widget_grab_focus (GTK_WIDGET (webview));
  
  // Make sure the main window and all its contents are visible
  gtk_widget_show_all (window);
  
  // Run the main GTK+ event loop.
  gtk_main();
}
