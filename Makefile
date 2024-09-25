# Compiler and flags
CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lX11 -lm

# Target executable
TARGET = main

# Build the target executable
all: $(TARGET)

$(TARGET): $(TARGET).o
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o $(LDFLAGS)

# Compile the source file into an object file
$(TARGET).o: $(TARGET).c
	$(CC) $(CFLAGS) -c $(TARGET).c

# Clean up build files
clean:
	rm -f $(TARGET) $(TARGET).o

