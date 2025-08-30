CC = gcc

CFLAGS = -Wall -Wextra -Iinclude

SRCS = src/main.c src/get_ext.c src/get_mime.c src/urldecode.c

OBJS = $(SRCS:.c=.o)

TARGET = server

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: clean
