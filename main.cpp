#include <QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("Influx");
  QCoreApplication::setOrganizationDomain("influx.kr");
  QCoreApplication::setApplicationName("Stitcher");
  QCoreApplication::setApplicationVersion("0.5.1");

  MainWindow mw;
  mw.show();

  return app.exec();
}
