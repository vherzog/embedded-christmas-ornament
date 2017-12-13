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
		timer->start(IMAGE_PERIOD); // cycle through images
	} else if(state == 2) {
		timer->start(FAST_PERIOD); // poll device driver
	} else {
		timer->start(DISPLAY_PERIOD); // turn off display
	}
	

}

void Timer::timer_handler() {
	/* If Timer 1 and the Gui is on, cycle through the next image */
	if(state == 1 && myGui->on) {
		//cout << "Change to next pic..." << endl;
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
	/* If Timer 0 and display is on, set the display to off and turn off music */
	else if(state == 0 && myGui->on){
		//cout << "TURN OFF SCREEN -- OTHER TIMER" << endl;
		system("/home/root/play_music.sh -p /media/card/mp3/jingle-bells.mp3");
		myGui->on = 0;
		myGui->myLabel2->hide();
		myGui->myLabel3->hide();
		myGui->myLabel1->hide(); 
		myGui->myLabel4->showFullScreen();
	} 
	/* If Timer 2, poll device driver and act accordingly */
	else if (state == 2){
		char* buffer = (char*) malloc(256);
		memset(buffer, 0, 256);
		int file;
  		file = open("/dev/sensor", O_RDWR);
		read(file, (void*) buffer, 256);
		int displayEn, speakerEn;
		sscanf(buffer, "%d %d", &displayEn, &speakerEn);
		cout << "BUFFER: " << buffer << endl;

		// If displayEn HIGH && off right now, turn on
		if(displayEn == 1 && !myGui->on) {
			// TURN SCREEN ON
			cout << "TURN ON SCREEN, count: " << count << endl;
			system("/home/root/play_music.sh -s /media/card/mp3/jingle-bells.mp3");
			myGui->on = 1;
			myGui->myLabel4->hide();
			timer->start(SLOW_PERIOD);
			close(file);
			return;
		}
		// If displayEn LOW && on right now, turn off
		else if(!displayEn && myGui->on) {
			// TURN SCREEN OFF
			cout << "TURN OFF SCREEN, count: " << count << endl;
			system("/home/root/play_music.sh -p");
			myGui->on = 0;
			myGui->myLabel4->showFullScreen();
			timer->start(FAST_PERIOD);
			cout << "displayEn " << displayEn << " && on right now, turn off" << endl;
			close(file);
			return;
		}
		// If displayEn HIGH && on right now, keep at slower timer
		else if(displayEn == 1 && myGui->on) {
			timer->start(SLOW_PERIOD);
			close(file);
			return;
		}
		// If displayEn LOW && off right now, keep at slower timer
		else if(!displayEn && !myGui->on) {
			timer->start(FAST_PERIOD);
			close(file);
			return;
		}
		close(file);
	}
}






