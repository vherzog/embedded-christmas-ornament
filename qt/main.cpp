//#include <QGuiApplication>
#include <QtGui/QApplication>
#include <QLabel>
#include <stdio.h>
#include <Gui.h>
#include <Timer.h>

int main(int argc, char *argv[])
{
	/* Create and run GUI application */
	QApplication app(argc, argv);
	QApplication::setOverrideCursor(Qt::BlankCursor);
	Gui* window = new Gui();

	Timer* t0 = new Timer(0, window);
	Timer* t1 = new Timer(1, window);
	Timer* t2 = new Timer(2, window);
	t0->setupTimer();
  	t1->setupTimer();
	t2->setupTimer();
	
	//window.showFullScreen();
	//window.show();
	
	return app.exec();
}




























