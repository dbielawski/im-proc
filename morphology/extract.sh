#!/bin/bash


./binarize 128 255 ../data/gear.ppm binarized_gear.ppm
#pvisu binarized_gear.ppm
./opening 2 3 binarized_gear.ppm opened_gear.ppm
#pvisu opened_gear.ppm
#./subtract binarized_gear.ppm opened_gear.ppm sub_gear.ppm
#pvisu sub_gear.ppm
#./labeling sub_gear.ppm gear_color.ppm