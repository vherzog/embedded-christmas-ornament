EC535 Final Project embedded-christmas-ornament
--------------
Embedded Christmas Ornament: final project for BU EC535: Embedded Systems class<br />

## Installation Instructions
### Bluetooth Configuration Instructions 
* Copy all *.ipk files in BluetoothPackages directory to the Gumstix board
* Use the command ipkg -d <DESTINATION> <filename>.ipk -force-depends    
    * Note that DESTINATION is a variable defined in the ipkg.conf file in the etc directory
* By using the -force-depends command the all necessary packages should be able to be installed without any regard to order of installation. This is hacky, however it worked for us.
* Determine the MAC address of your speaker
* In the /etc/asound.conf file create a struct like the one below


>pcm.speaker{
>    type bluetooth;
>   device “MAC Address”;
>};


### Bluetooth Connection Instructions
Find the MAC address of Bluetooth speaker and replace with the <MAC Address> in /startup/startup_script.sh<br/>

### Make 
cd qt/<br />
qmake<br />
Edit the Makefile to change the INC Path to the following:<br /><br />
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
              -I/ad/eng/courses/ec/ec535/gumstix/oe/qt/mkspecs/qws/linux-arm-g++ -I. -I. -I. -I.<br /><br />
make<br />
cd ../km/<br />
make<br />

### Install and run executables
Move the following executables to the home Gumstix directory. Note that if all files below are in the correct directories the system will run without any additional user input.
* km/sensor.ko
* qt/qt
* mp3/play_music.sh
*  scripts/startup_script.sh
*  qt/ornament.png
*  qt/wreath.png
*  qt/xmas-tree.png
* mp3 file to the SD Card (/media/card/mp3/jingle-bells.mp3)


