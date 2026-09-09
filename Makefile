CC ?= clang
CFLAGS ?= -Wall -Wextra -Wpedantic -O2

TARGET := ProxyGPT.app/Contents/MacOS/launcher
SOURCES := main.c dotenv.c
HEADERS := dotenv.h

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
	chmod +x $(TARGET)

clean:
	rm -f $(TARGET)
