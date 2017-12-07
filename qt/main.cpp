#include <QtGui/QApplication>
#include <QLabel>
#include <stdio.h>
#include <Gui.h>

int main(int argc, char *argv[])
{
	/* Create and run GUI application */
	QApplication app(argc, argv);
	Gui window;
	window.showFullScreen();
	window.show();
	
	return app.exec();
}




























