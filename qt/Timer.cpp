#include <Timer.h>


Timer::Timer(int st, Gui* gui) {
	state = st;
	myGui = gui;
}

void Timer::setupTimer() {
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timer_handler()));

}

void Timer::timer_handler() {
	// Image timer, change to next image_idx
	if(state) {
		cout << "Turn off..." << endl;
		img_idx++;
		if (img_idx == IMG_TOTAL) img_idx = 0;
		switch(img_idx) {
			case 0: // Ornament
				myLabel2->hide();
				myLabel3->hide();
				myLabel1->show(); 
			case 1: // Xmas tree
				myLabel1->hide(); 
				myLabel2->show();
				myLabel3->hide();
			case 2: // Wreath
				myLabel1->hide(); 
				myLabel2->hide();
				myLabel3->show();
	}
	// Display timer, time to turn off the display!
	else {
		//TODO Turn off the display
		cout << "Turn off..." << endl;
	}
}

