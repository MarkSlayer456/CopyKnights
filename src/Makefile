IDIR = ../include
CC=gcc
CFLAGS=-I$(IDIR) -Wall

ODIR=obj
LDIR=../lib

LIBS=-lncurses

_DEPS = game_manager.h items.h player.h hud.h enemy.h functions.hmap_manager.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = game_manager.o main.o player.o hud.o enemy.o functions.omap_manager.o items.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ck: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
