CC = gcc
CFLAGS = -Wall -Wextra

SRC = 3.c
BUILD_DIR = make-build
EXEC = $(BUILD_DIR)/env

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean