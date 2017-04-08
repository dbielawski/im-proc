ROOT=../bcl

CFLAGS=-g -o0 -std=c99 -I$(ROOT)/include 
# CFLAGS=-Wall -Wextra -Werror -std=c99 -I$(ROOT)/include 
LDFLAGS=-L$(ROOT)/lib
LDLIBS=-lm -lfftw3 -lbcl

OBJ=fft.o
BIN=heat-equation\
	anisotropic-diffusion\
	median\
	nl-means\
	butterworth

all: $(BIN)

butterworth: fft.o

clean:
	$(RM) $(OBJ) $(BIN) *.ppm

.PHONY: all clean 
