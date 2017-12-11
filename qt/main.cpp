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
	//system("passkey-agent 0000 10:B7:F6:06:F9:82 &");
	//system("aplay -D d80&");
	cout << "STARTING FIRST SONG" << endl;
	system("/home/root/play_music.sh -s JoyToTheWorld.mp3");

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




























