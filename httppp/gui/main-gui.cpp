#include <QApplication>
#include "gui/httpppmainwindow.h"
#include "global.h"

int main(int argc, char *argv[]) {
  QApplication::setStyle("cleanlooks");
  QApplication a(argc, argv);
  HttpppMainWindow w;
  w.setWindowTitle(QObject::tr("httppp version %1", "window title")
                   .arg(VERSION));
  w.show();
  if (argc > 1)
    w.loadFile(argv[1]);
  return a.exec();
}
