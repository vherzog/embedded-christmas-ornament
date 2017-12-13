# embedded-christmas-ornament
Embedded Christmas Ornament: final project for BU EC535: Embedded Systems class

## Installation Instructions
### Installing necessary Bluetooth libraries

### Set up Bluetooth
Find the MAC address of Bluetooth speaker and replace with the <MAC Address> in /startup/startup_script.sh

### Make 
cd qt/
qmake
Edit the Makefile to change the INC Path to the following:
    INCPATH = -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++ \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/arm-angstrom-linux-gnueabi \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/backward \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/bits \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/debug \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/ext \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include/QtCore \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include/QtGui \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include/QtNetwork \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include \
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/mkspecs/qws/linux-arm-g++ -I. -I. -I. -I.
make
cd ../km/
make

### Install and run executables
Move the following executables to the home Gumstix directory:
  km/mygpio.ko
  qt/qt
  mp3/play_music.sh
  scripts/startup_script.sh
  qt/ornament.png
  qt/wreath.png
  qt/xmas-tree.png
Move the mp3 file to the SD Card (/media/card/mp3/jingle-bells.mp3)
