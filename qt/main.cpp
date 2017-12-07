#include <QtGui/QApplication>
#include <QLabel>
#include <stdio.h>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>


int main(int argc, char *argv[])
{
    //printf("open!\n");
    QApplication a(argc, argv);
    QImage myImage;
    myImage.load("ornament.png");
    //QLabel label("<img src=':/home/root/ornament.png' />");
    //label.show();
    //QPixmap pic("/home/root/ornament.png");

    QLabel* myLabel = new QLabel();
    myLabel->setPixmap(QPixmap::fromImage(myImage));

    //myLabel->setPixmap(pic);

    myLabel->show();

    return a.exec();
}

/*    QApplication a(argc, argv);
    QGraphicsScene scene;
    QGraphicsView view(&scene);
    QPixmap* img = new QPixmap();
    img->fill();
//    QGraphicsPixmapItem item(QPixmap("/home/root/ornament.png"));
    QGraphicsPixmapItem item(*img);
    scene.addItem(&item);
    view.show();*/

    /*QApplication a(argc, argv);

    //QImage myImage;
    QPicture* myImage = new QPicture();
    myImage->load("ornament.png");

    QLabel* myLabel = new QLabel();
    //myLabel.setPixmap(QPixmap::fromImage(myImage));
    myLabel->setPicture(*myImage);

    myLabel->show();

    return a.exec();*/




























