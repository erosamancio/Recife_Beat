TARGET = jogo
CC = gcc
SRC = main.c telas.c ranking.c mapa.c pontos.c controles.c
CFLAGS = -Wall -std=c99
LDFLAGS = -lraylib

ifeq ($(OS),Windows_NT)
	TARGET = jogo.exe
	LDFLAGS += -lgdi32 -lwinmm
else
	UNAME_S := $(shell uname -s)

	# linux
	ifeq ($(UNAME_S),Linux)
		LDFLAGS += -lGL -lm -lpthread -ldl -lrt -lX11
	endif

	# mac
	ifeq ($(UNAME_S),Darwin)
		LDFLAGS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	endif
endif

all: build run

build:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

run:
	./$(TARGET)

clean:
	rm -f $(TARGET)