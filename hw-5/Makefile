CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -fsanitize=leak # -DDEBUG

SRC = 1.c circular_buffer.h linked_list.h
BUILD_DIR = make-build
EXEC = $(BUILD_DIR)/taxi

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean