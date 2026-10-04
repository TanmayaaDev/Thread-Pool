CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude -O3
LDFLAGS = -lpthread

TARGET = tests/test_pool
SRC = src/threadpool.c tests/test_pool.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean