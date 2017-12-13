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

	/* Start to play song */
	//cout << "STARTING FIRST SONG" << endl;
	system("/home/root/play_music.sh -s JoyToTheWorld.mp3");

	/* Set control timers */
	Timer* t0 = new Timer(0, window);	// Turns off screen after 10 minutes
	Timer* t1 = new Timer(1, window);	// Cycles through images every 2 seconds
	Timer* t2 = new Timer(2, window);	// Polls device driver dynamically
	t0->setupTimer();
  	t1->setupTimer();
	t2->setupTimer();
	
	return app.exec();
}




























