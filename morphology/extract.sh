#!/bin/bash

if [ $# -ne 1 ]; then
	echo "1 argument needed";
	exit -1;
fi

./binarize 128 255 $1 binarized_gear.ppm
pvisu binarized_gear.ppm
./opening 2 10 binarized_gear.ppm opened_gear.ppm
pvisu opened_gear.ppm
./subtract binarized_gear.ppm opened_gear.ppm sub_gear.ppm
pvisu sub_gear.ppm
./labeling-color sub_gear.ppm colored_gear.ppm
pvisu colored_gear.ppm