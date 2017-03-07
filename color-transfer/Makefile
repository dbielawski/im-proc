ROOT=../bcl

# CFLAGS=-Wall -Wextra -Werror -std=c99 -I$(ROOT)/include 
CFLAGS=-g -o0 -std=c99 -I$(ROOT)/include 
LDFLAGS=-L$(ROOT)/lib -lbcl 
LDLIBS=-lm -lbcl 

OBJ=fft.o 
BIN=color-transfer

all: $(BIN)

# padding:

clean:
	$(RM) $(OBJ) $(BIN) *.ppm

.PHONY: all clean 
