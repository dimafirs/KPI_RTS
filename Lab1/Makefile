CC = gcc
CFLAGS = -O0 -Wall -g
TARGET = main
OBJ = main.o

.PHONY: clean

all: clean $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lm
clean:
	rm -rf $(OBJ) $(TARGET) *_values
