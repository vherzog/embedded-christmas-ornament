#include <Timer.h>

extern int img_idx;


Timer::Timer(int st, Gui* gui) {
	state = st;
	myGui = gui;
}

void Timer::setupTimer() {
	timer = new QTimer(this);
	timer->setInterval(1000);
	connect(timer, SIGNAL(timeout()), this, SLOT(timer_handler()));

	if(state) {
		timer->start(2000);
	} else {
		timer->start(1000000);
	}

}

void Timer::timer_handler() {
	// Image timer, change to next image_idx
	if(state) {
		cout << "Change to next pic..." << endl;
		img_idx++;
		if (img_idx == IMG_TOTAL) img_idx = 0;
		switch(img_idx) {
			case 0: // Ornament
				myGui->myLabel2->hide();
				myGui->myLabel3->hide();
				myGui->myLabel1->showFullScreen(); 
				break;
			case 1: // Xmas tree
				myGui->myLabel1->hide(); 
				myGui->myLabel3->hide();
				myGui->myLabel2->showFullScreen();
				break;
			case 2: // Wreath
				myGui->myLabel1->hide(); 
				myGui->myLabel2->hide();
				myGui->myLabel3->showFullScreen();
				break;
		}
	}
	// Display timer, time to turn off the display!
	else {
		//TODO Turn off the display
		cout << "Turn off..." << endl;
	}
}

