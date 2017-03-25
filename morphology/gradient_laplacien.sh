#!/bin/bash
if [ $# -ne 2 ]; then
	echo "2 arguments needed";
	exit -1;
fi

# grdient morphologique
./dilation 	$1 $2 ../data/lena-gray.ppm a.ppm
./erosion 	$1 $2 ../data/lena-gray.ppm b.ppm
./subtract	a.ppm b.ppm gradient.ppm

# laplacien
./add		a.ppm b.ppm d.ppm
./subtract	d.ppm ../data/lena-gray.ppm e.ppm
./subtract 	e.ppm ../data/lena-gray.ppm laplacien.ppm
