#include "QtWidgetsApplication3.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
  
    QApplication a(argc, argv);
    QtWidgetsApplication3 w;
    w.show();
    return a.exec();
}
