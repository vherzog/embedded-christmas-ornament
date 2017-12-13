#!/bin/bash

case "$1" in
	# Case: Start playing new song
	-s)
	echo start $2
	madplay $2 -r 44100 --output=wave:- | aplay -D d80 & > output
	;;

	# Case: Stop playing current song
	-p)
	echo stop $2
	killall madplay
	;;

esac
