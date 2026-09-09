CC ?= clang
CFLAGS ?= -Wall -Wextra -Wpedantic -O2

TARGET := ProxyGPT.app/Contents/MacOS/launcher
SOURCE := main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TARGET)
	chmod +x $(TARGET)

clean:
	rm -f $(TARGET)
