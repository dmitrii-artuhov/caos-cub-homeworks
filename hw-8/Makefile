CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -fsanitize=leak # -DDEBUG

SRC = 3.c
BUILD_DIR = make-build
EXEC = $(BUILD_DIR)/mwc

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean