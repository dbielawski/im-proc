ROOT=../bcl

CFLAGS=-Wall -Wextra -Werror -std=c99 -I$(ROOT)/include
LDFLAGS=-L$(ROOT)/lib -lbcl 
LDLIBS=-lm -lbcl 

BIN=color-transfer

all: $(BIN)

# padding:

clean:
	$(RM) $(BIN) *.ppm

.PHONY: all clean 
