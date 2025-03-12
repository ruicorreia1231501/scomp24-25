CC = gcc
CFLAGS = -Wall -Wextra
TARGET = prog
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)
clean:
	rm -f $(TARGET)