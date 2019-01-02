CC = gcc
CFLAGS = -g  `sdl2-config --cflags` -O3
LDFLAGS = `sdl2-config --libs` -lSDL2_mixer -lSDL2_image

CFILES = $(wildcard *.c)
HFILES = $(CFILES:.c=.h)
OBJS = $(CFILES:.c=.o)
EXE = ivdx

%.o: %.c
	$(CC) $(CFLAGS) -c $<

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)


.PHONY: clean
clean:
	del $(OBJS) $(EXE)
