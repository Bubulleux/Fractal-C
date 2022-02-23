#!/bin/bash

RED="\033[1;31m"
GREEN="\e[1;32m"
NOCOLOR="\033[0m"

SRC="src/"
INCLUDE="./ -L/usr/X11R6/lib -lX11 -lm -lOpenCL"

SRC_FILE="${SRC}*.c"
FILES=""
for f in $SRC_FILE
do
	FILES="$FILES $f"
done
rm main

CMD="gcc  -o main $FILES -g -I $INCLUDE"
echo $CMD \n
$CMD

if [ $? -ne 0 ]; then
	echo -e "${RED}Compilation Fail${NOCOLOR}"
else
	echo -e "${GREEN}Compilation Finish${NOCOLOR}\n"
    echo "Run programe (y/n/d)"

    select yn in "Yes" "No" "Debug"; do
        case $yn in
            Yes ) ./main; break;;
            No ) exit;;
            Debug ) gdb ./main; break;;
            *) exit;;
         esac
    done
fi
