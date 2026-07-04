CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS =
SRC = main.c
TARGET = rename_tool

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
