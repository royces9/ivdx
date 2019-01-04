CC = gcc
CFLAGS = -g  `sdl2-config --cflags` -O3
LDFLAGS = `sdl2-config --libs` -lSDL2_mixer -lSDL2_image

CFILES = $(wildcard *.c)
HFILES = $(CFILES:.c=.h)
OBJS = $(CFILES:.c=.o)
EXE = ivdx

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(EXE): $(OBJS) $(CFILES)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)


.PHONY: clean
clean:
	del $(OBJS) $(EXE)

kill:
	kill -9 $$(ps aux | grep "./$(EXE)" | head -n 1 | cut -b10-15)
