#include <Timer.h>

extern int img_idx;
extern int prev_on = 1;


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
	/*if(prev_on && !myGui->on) {
		cout << "STOP MUSIC" << endl;
		system("/home/root/play_music.sh -p");
	} else if(!prev_on && myGui->on) {
		cout << "START MUSIC" << endl;
		system("/home/root/play_music.sh -s /media/card/mp3/jingle-bells.mp3");
	}
	prev_on = myGui->on;*/
	static int count = 0;
	// If the screen should be on
	// Image timer, change to next image_idx
	count++;
	cout << "COUNT start: " << count << endl;
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
	// Display timer, time to turn off the display!
	else if(state == 0 && myGui->on){
		cout << "TURN OFF SCREEN -- OTHER TIMER" << endl;
		system("/home/root/play_music.sh -p /media/card/mp3/jingle-bells.mp3");
		myGui->on = 0;
		myGui->myLabel2->hide();
		myGui->myLabel3->hide();
		myGui->myLabel1->hide(); 
		myGui->myLabel4->showFullScreen();
	} else if (state == 2){
		char* buffer = (char*) malloc(256);
		memset(buffer, 0, 256);
		int file;
  		file = open("/dev/sensor", O_RDWR);
		read(file, (void*) buffer, 256);
		int displayEn, speakerEn;
		//displayEn = 0;
		//speakerEn = 0;
		sscanf(buffer, "%d %d", &displayEn, &speakerEn);
		cout << "BUFFER: " << buffer << endl;

		// If displayEn HIGH && off right now, turn on
		if(displayEn == 1 && !myGui->on) {
			// TURN SCREEN ON
			cout << "TURN ON SCREEN, count: " << count << endl;
			system("/home/root/play_music.sh -s /media/card/mp3/jingle-bells.mp3");
			myGui->on = 1;
			myGui->myLabel4->hide();
			timer->start(5000);
			//cout << "displayEn " << displayEn << " && off right now, turn on" << endl;
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
			timer->start(200);
			cout << "displayEn " << displayEn << " && on right now, turn off" << endl;
			close(file);
			return;
		}
		// If displayEn HIGH && on right now, keep at slower timer
		else if(displayEn == 1 && myGui->on) {
			timer->start(20000);
			//cout << "displayEn " << displayEn << " && on right now, keep at slower timer" << endl;
			close(file);
			return;
		}
		// If displayEn LOW && off right now, keep at slower timer
		else if(!displayEn && !myGui->on) {
			timer->start(200);
			//cout << "displayEn " << displayEn << " && off right now, keep at slower timer" << endl;
			close(file);
			return;
		}
		close(file);
	}
}






