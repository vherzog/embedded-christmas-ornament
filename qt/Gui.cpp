#include <Gui.h>

extern int img_idx = 0;

/* Gui Constructor */
Gui::Gui(QWidget* parent) : QWidget(parent) {

	on = 1; // Start as on
	
	/* Create Qlabels to display images */
	myLabel1 = new QLabel("<img src='ornament.png' />");
	myLabel2 = new QLabel("<img src='xmastree.png' />");
	myLabel3 = new QLabel("<img src='wreath.png' />");
	myLabel4 = new QLabel;
	myLabel4->setStyleSheet("QLabel { background-color : black; color : black }");
 	
 	/* Align images in center of display */
	myLabel1->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	myLabel2->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	myLabel3->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	/* On start, show the first image */
	myLabel2->hide();
	myLabel3->hide();
	myLabel4->hide();
    	myLabel1->showFullScreen();
}

