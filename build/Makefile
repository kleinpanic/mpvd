CC = gcc
CFLAGS = -I../include -Wall -g
LDFLAGS = -lasound -lncurses  # Added -lncurses for ncurses functions
OBJDIR = ../obj
SRCDIR = ../src
INCLUDEDIR = ../include

OBJS = $(OBJDIR)/main.o $(OBJDIR)/audio.o $(OBJDIR)/ui.o $(OBJDIR)/dir.o

all: mpvd

mpvd: $(OBJS)
	$(CC) $(CFLAGS) -o mpvd $(OBJS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o mpvd
