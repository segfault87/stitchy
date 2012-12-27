#include <QApplication>

#include "common.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("Influx");
  QCoreApplication::setOrganizationDomain("influx.kr");
  QCoreApplication::setApplicationName("Stitchy");
  QCoreApplication::setApplicationVersion(VERSION);

  MainWindow mw;
  mw.show();

  return app.exec();
}
