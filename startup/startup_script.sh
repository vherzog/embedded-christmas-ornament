#!/bin/bash

cd /usr/lib
	ln -s /media/card/lib/libQtCore.so.4 libQtCore.so.4
	ln -s /media/card/lib/libQtGui.so.4 libQtGui.so.4
	ln -s /media/card/lib/libQtNetwork.so.4 libQtNetwork.so.4
	ln -s /media/card/lib/ld-uClibc.so.0 ld-uClibc.so.0
	ln -s /media/card/lib/libc.so.0 libc.so.0
	ln -s /media/card/lib/libm.so.0 libm.so.0
	ln -s /media/card/lib/libstdc\+\+.so.6 libstdc\+\+.so.6

cd /home/root/
export QWS_MOUSE_PROTO='tslib:/dev/input/touchscreen0'
	export TSLIB_CONFFILE=/etc/ts.conf
	export TSLIB_PLUGINDIR=/usr/lib
	export TSLIB_TSDEVICE=/dev/input/event0
	export TSLIB_FBDEVICE=/dev/fb0
	export TSLIB_CONSOLEDEVICE=/dev/tty
	export QT_QWS_FONTDIR=/media/card/lib/fonts
	export TSLIB_PLUGINDIR=/usr/lib/ts

passkey-agent 0000 <MAC Address> &
aplay -D d80&

mknod /dev/sensor c 61 0
insmod sensor.ko

./qt -qws -nomouse &
