#include <Gui.h>

int image_idx = 0;

/* Gui Constructor */
Gui::Gui(QWidget* parent) : QWidget(parent) {
	/* Create list of images loaded on SD Card */
	/*imgs = new QListWidget(this);
	img_idx = 0;

	dirName = "/media/card/pictures/";
	dir.setPath(dirName);
	dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
	
	imgList = new QStringList();
	*imgList = dir.entryList();*/
	
	/* Add images to a QListWidget to be accessed */
	/*img_total = imgList->size();
	for(int i = 0; i < img_total; i++) {
		QListWidgetItem *itm = new QListWidgetItem();
		itm->setText((*imgList)[i]);
		imgs->insertItem(i, itm);
		qDebug() << (*imgList)[i];
	}*/

	/* Creat grid layout to display widget */
	//layout = new QGridLayout(this);
	
	/* Create QImage to display current image */
	//TODO 
	myLabel1 = new QLabel("<img src='ornament.png' />");
	myLabel2 = new QLabel("<img src='xmastree.png' />");
	myLabel3 = new QLabel("<img src='wreath.png' />");
 
    	myLabel1->show(); 
	myLabel2->hide();
	myLabel3->hide();
 


	/*Set timer to switch between images */
	//TODO timer, connect() to image_handler()

	/* Add widgets to layout */
	
	

}

/* Switches between images loaded on SD Card */
/*void Gui::image_handler() {
	//TODO switch images...
	img_idx++;
	if (img_idx == img_total) img_idx = 0;

}*/
