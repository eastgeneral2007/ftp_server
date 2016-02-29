#!/bin/bash
P=./running 
G=

if [ "$*" = "d" ] ; then
	G=gdb
	sudo $G ./bin/server.out 
	exit 0
fi

if [ "$*" = "kill" ] ; then
	echo "killing server"
	sudo pkill server.out
	exit 0
fi

if [ -f $P ] ; then
	echo "already running -> restarting "
	sudo pkill server.out
else
	echo "running new instance"
	touch  $P
fi

(
sudo ./bin/server.out 
rm $P
) &
