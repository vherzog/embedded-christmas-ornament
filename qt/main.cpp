#include <QtGui/QApplication>
#include <QLabel>
#include <stdio.h>
#include <Gui.h>
#include <Timer.h>

int main(int argc, char *argv[])
{
	/* Create and run GUI application */
	QApplication app(argc, argv);
	Gui* window = new Gui();

	Timer* t = new Timer(1, window);
  	t->setupTimer();
	
	//window.showFullScreen();
	//window.show();
	
	return app.exec();
}




























