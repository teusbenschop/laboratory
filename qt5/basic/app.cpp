// https://wiki.qt.io/Qt_for_Beginners

#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>

int main (int argc, char **argv)
{
  QApplication app (argc, argv);
  
  QPushButton button1;
  //button.resize (200, 60);
  button1.setText ("Button text");
  button1.setToolTip ("Button tooltip");

  QFont font ("Courier");
  button1.setFont (font);

  QIcon icon ("icon100.png");
  button1.setIcon (icon);

  //QPushButton button2 ("other", &button1);

  button1.show();
  
  QObject::connect(&button1, SIGNAL (clicked()), QApplication::instance(), SLOT (quit()));

  int exitcode = app.exec ();
  
  return exitcode;
}

