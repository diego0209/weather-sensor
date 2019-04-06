#!/bin/bash

if [ $# -gt 0 ]
then
	FILE=$1 # name = first parameter
	if [ -f $FILE ]
	then
		cat headers.csv $FILE > tmp
		mv tmp $FILE
	else
		echo "File $FILE does not exist."
	fi
else
	echo "No file specified."
fi
