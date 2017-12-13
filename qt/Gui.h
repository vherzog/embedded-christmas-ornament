#ifndef GUI_H
#define GUI_H


#include <QWidget>
#include <QtGui>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QGridLayout>
#include <stdlib.h>
#include <QDir>
#include <QString>
#include <QStringList>
#include <string>
#include <QImage>

#define IMG_TOTAL 3


/* Gui class - used to display images to LCD screen connected to Gumstix board */
class Gui;

class Gui : public QWidget {
	Q_OBJECT
public:
	int on;
	Gui(QWidget *parent = 0);
	QLabel* myLabel1;
	QLabel* myLabel2;
	QLabel* myLabel3;
	QLabel* myLabel4;

private:
	/* Declare image widget that will be displayed */
	QImage image;
};



#endif
