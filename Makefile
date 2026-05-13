CC = gcc

CFLAGS = -Wall -Wextra -std=c99

SRC = main.c mapa.c ranking.c

OUT = recife_beat

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Linux)

LIBS = -lraylib -lm -ldl -lpthread

endif

ifeq ($(UNAME_S),Darwin)

LIBS = -lraylib \
	   -framework OpenGL \
	   -framework Cocoa \
	   -framework IOKit \
	   -framework CoreAudio \
	   -framework CoreVideo

endif

all:
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LIBS)

run: all
	./$(OUT)

clean:
	rm -f $(OUT)