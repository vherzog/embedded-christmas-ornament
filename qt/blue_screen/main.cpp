#include <QApplication>
#include <QWidget>
#include <QPalette>
//#include <QTimer>
#include <QLabel>
//#include <stdio.h>

int main(int argc, char **argv){
  QApplication app(argc, argv);
  QWidget w;
  QPalette p = w.palette();
  p.setColor(QPalette::Window, Qt::blue);
  w.setPalette(p);
  w.setAutoFillBackground(true);

  QLabel label;
  w.move(100,100);
  w.resize(25,25);
  label.setText("hi");
  w.showFullScreen();
  return app.exec();
}
