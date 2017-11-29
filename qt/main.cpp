#include <QtGui/QApplication>
#include <QLabel>
#include <stdio.h>
//#include <QPicture>

int main(int argc, char *argv[])
{
    printf("open!\n");
    QApplication a(argc, argv);
    QLabel label("<img src=':/home/root/ornament.png' />");
    label.show();
    return a.exec();
    /*QApplication a(argc, argv);

    //QImage myImage;
    QPicture* myImage = new QPicture();
    myImage->load("ornament.png");

    QLabel* myLabel = new QLabel();
    //myLabel.setPixmap(QPixmap::fromImage(myImage));
    myLabel->setPicture(*myImage);

    myLabel->show();

    return a.exec();*/
}




























