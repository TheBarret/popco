CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -O2 $(shell pkg-config --cflags sdl2 SDL2_ttf 2>/dev/null)
LDFLAGS := $(shell pkg-config --libs sdl2 SDL2_ttf 2>/dev/null) -lm

SRCS := main.c config.c stats.c render.c crt_effects.c
OBJS := $(SRCS:.c=.o)
TARGET := monitor

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
