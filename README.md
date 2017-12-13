EC535 Final Project embedded-christmas-ornament
--------------
Embedded Christmas Ornament: final project for BU EC535: Embedded Systems class<br />
Contributors: John Abernathy & Veronica Herzog

## Installation Instructions
### Bluetooth Configuration Instructions 
* Copy all *.ipk files in BluetoothPackages directory to the Gumstix board
* Use the command ipkg -d <DESTINATION> <filename>.ipk -force-depends    
    * Note that DESTINATION is a variable defined in the ipkg.conf file in the etc directory
* By using the -force-depends command the all necessary packages should be able to be installed without any regard to order of installation. This is hacky, however it worked for us.
* Determine the MAC address of your speaker using "hcitool scan"
* In the /etc/asound.conf file create a struct like the one below


>pcm.speaker{
>    type bluetooth;
>   device “MAC Address”;
>};


### Bluetooth Connection Instructions
Find the MAC address of Bluetooth speaker and replace with the MAC Address in /startup/startup_script.sh<br/>

### Make 
cd qt/<br />
qmake<br />
Edit the Makefile to change the INC Path to the following:<br /><br />
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
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/mkspecs/qws/linux-arm-g++ -I. -I. -I. -I.<br />
make<br />
cd ../km/<br />
make<br />

### Install and run executables
Move the following executables to the home Gumstix directory (/home/root). Note that if all files below are in the correct directories the system will run without any additional user input.
* km/sensor.ko
* qt/qt
* mp3/play_music.sh
*  scripts/startup_script.sh
*  qt/ornament.png
*  qt/wreath.png
*  qt/xmas-tree.png
* mp3 file to the SD Card (/media/card/mp3/jingle-bells.mp3)


