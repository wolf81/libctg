CC = gcc
MODE ?= debug

ifeq ($(MODE),release)
	CFLAGS = -O2 -std=c99
	BUILDDIR = build/release
else
	CFLAGS = -g -Wall -Wextra -std=c99
	BUILDDIR = build/debug
endif

SRCDIR = src
TARGET = $(BUILDDIR)/ctg_main
OBJS = $(BUILDDIR)/main.o $(BUILDDIR)/libctg.o

.PHONY: all clean debug release run

all: $(TARGET)

debug:
	$(MAKE) MODE=debug

release:
	$(MAKE) MODE=release

run: $(TARGET)
	./$(TARGET)

$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(TARGET): $(BUILDDIR) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BUILDDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR)/libctg.o: $(SRCDIR)/libctg.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build
