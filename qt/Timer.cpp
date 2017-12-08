#include <Timer.h>

extern int img_idx;


Timer::Timer(int st, Gui* gui) {
	state = st;
	myGui = gui;
}

void Timer::setupTimer() {
	timer = new QTimer(this);
	timer->setInterval(10000000);
	connect(timer, SIGNAL(timeout()), this, SLOT(timer_handler()));

	if(state == 1) {
		timer->start(2000); // cycle through images
	} else if(state == 2) {
		timer->start(200); // poll device driver
	} else {
		timer->start(600000); // turn off display
	}
	

}

void Timer::timer_handler() {
	// If the screen should be on
	// Image timer, change to next image_idx
	if(state == 1 && myGui->on) {
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
	else if(state == 0 && myGui->on){
		//TODO Turn off the display
		cout << "Turn off dislpay..." << endl;
		myGui->on = 0;
		myGui->myLabel2->hide();
		myGui->myLabel3->hide();
		myGui->myLabel1->hide(); 
		myGui->myLabel4->showFullScreen();
	} else if (state == 2){
		//TODO poll device driver to determine whether to turn back on or not
		char* buffer = (char*) malloc(512);
		int file;
  		file = open("/dev/mygpio", O_RDWR);
		read(file, (void*) buffer, 512);
		int displayEn, speakerEn;
		sscanf(buffer, "%d %d", &displayEn, &speakerEn);
		//cout << "displayEn = " << displayEn << ", speakerEn = " << speakerEn << endl;

		// If displayEn HIGH && off right now, turn on
		if(displayEn && !myGui->on) {
			// TURN SCREEN ON
			myGui->on = 1;
			myGui->myLabel4->hide();
			timer->start(33000);
			cout << "displayEn HIGH && off right now, turn on" << endl;
		}
		// If displayEn LOW && on right now, turn off
		if(!displayEn && myGui->on) {
			myGui->on = 0;
			myGui->myLabel4->showFullScreen();
			timer->start(200);
			cout << "displayEn LOW && on right now, turn off" << endl;
		}
		// If displayEn HIGH && on right now, keep at slower timer
		if(displayEn && myGui->on) {
			timer->start(33000);
			cout << "displayEn HIGH && on right now, keep at slower timer" << endl;
		}
		// If displayEn LOW && off right now, keep at slower timer
		if(!displayEn && !myGui->on) {
			timer->start(200);
			cout << "displayEn LOW && off right now, keep at slower timer" << endl;
		}
	}
}






