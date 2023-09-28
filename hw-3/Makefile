CC = gcc
CFLAGS = -Wall -Wextra

SRC = 2.c
BUILD_DIR = make-build
EXEC = $(BUILD_DIR)/perfect

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean