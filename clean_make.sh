#!/bin/bash
if [ "$(uname)" == "Darwin" ]; then
    make clean
	make
elif [ "$(expr substr $(uname -s) 1 6)" == "CYGWIN" ]; then
	echo "build in CYGWIN"
	make -f makefile_cygwin clean
	make -f makefile_cygwin
elif [ "$(uname)" == "Linux" ]; then
	make -f makefile_linux clean
	make -f makefile_linux
else
	echo "unknow os!"
fi