#ifndef TIMER_H
#define TIMER_H

#include <QtCore/QObject>
#include <QTimer>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <Gui.h>
#include <iostream>

using namespace std;

#define DISPLAY_PERIOD 	180000		/* 180000ms = 3 minutes */
#define IMAGE_PERIOD 	30000		/* 30000ms = 30 seconds */


/* Timer class used to switch between images, and turn display off after period of time */
class Timer : public QObject {
	Q_OBJECT
public:
	Timer(int st, Gui* gui);
	void setupTimer();
	int state;		/* 0: display, 1: image, 2:  */
	QTimer* timer;
	Gui* myGui;

public Q_SLOTS:
	void timer_handler();
};




#endif
