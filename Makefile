CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lncurses
SRC = src/pm.c src/pmtui.c src/stack.c
OBJ = $(SRC:.c=.o)
TARGET = pm

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
