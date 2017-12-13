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

#define DISPLAY_PERIOD 	600000		/* 600000ms = 10 minutes */
#define IMAGE_PERIOD 	2000		/* 2000ms = 2 seconds */
#define FAST_PERIOD 	200			
#define SLOW_PERIOD		20000


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
