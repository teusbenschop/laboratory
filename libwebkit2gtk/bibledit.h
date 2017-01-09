#include <cstdlib>
#include <gtk/gtk.h>


using namespace std;


int main (int argc, char *argv[]);
static void activate (GtkApplication *app);
GtkApplication *application;
GtkWidget * window;
static void on_signal_destroy (gpointer user_data);
gint status;

