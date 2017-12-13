# embedded-christmas-ornament
Embedded Christmas Ornament: final project for BU EC535: Embedded Systems class<br />

## Installation Instructions
### Installing necessary Bluetooth libraries

### Set up Bluetooth
Find the MAC address of Bluetooth speaker and replace with the <MAC Address> in /startup/startup_script.sh<br />

### Make 
cd qt/<br />
qmake<br />
Edit the Makefile to change the INC Path to the following:<br />
    INCPATH = -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++ \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/arm-angstrom-linux-gnueabi \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/backward \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/bits \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/debug \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/cross/arm-angstrom-linux-gnueabi/include/c++/ext \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include/QtCore \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include/QtGui \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include/QtNetwork \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/include \<br />
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/mkspecs/qws/linux-arm-g++ -I. -I. -I. -I.<br />
make<br />
cd ../km/<br />
make<br />

### Install and run executables
Move the following executables to the home Gumstix directory:<br />
  km/mygpio.ko<br />
  qt/qt<br />
  mp3/play_music.sh<br />
  scripts/startup_script.sh<br />
  qt/ornament.png<br />
  qt/wreath.png<br />
  qt/xmas-tree.png<br />
Move the mp3 file to the SD Card (/media/card/mp3/jingle-bells.mp3)<br />
