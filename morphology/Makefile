ROOT=../bcl

# CFLAGS=-o3 -Wall -Wextra -Werror -std=c99 -I$(ROOT)/include 
CFLAGS=-g -o0 -Wall -Wextra -Werror -std=c99 -I$(ROOT)/include 
LDFLAGS=-L$(ROOT)/lib 
LDLIBS=-lm -lbcl

OBJ=morphology.o dilation.o erosion.o opening.o closing.o add.o subtract.o\
	binarize.o labeling-color.o

BIN=\
	labeling-color\
	binarize\
	add\
	subtract\
	opening\
	closing\
	dilation\
	erosion\
	make-se\
	labeling

all: $(BIN)

make-se: morphology.o
dilation: morphology.o
erosion: morphology.o
opening: morphology.o
closing: morphology.o

clean:
	$(RM) $(OBJ) $(BIN) *.ppm

.PHONY: all clean 
