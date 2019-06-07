
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>


int main (int argc, char **argv)
{
  QApplication app (argc, argv);
  
  QPushButton button ("Hello world!", 0);
  button.resize (100, 30);
  button.show();
  
  int exitcode = app.exec ();
  
  return exitcode;
}

