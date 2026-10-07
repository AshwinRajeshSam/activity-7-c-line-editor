CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -Iinclude
SRC = src/main.c src/editor.c
TARGET = editor

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET) *.exe document.txt

run: $(TARGET)
	./$(TARGET)
