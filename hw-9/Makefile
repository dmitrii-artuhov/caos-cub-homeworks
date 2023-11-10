CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -fsanitize=leak # -DDEBUG

SRC = src/quiz.c src/quiz.h src/fetch.c src/parse.c src/play.c
BUILD_DIR = make-build
EXEC = $(BUILD_DIR)/quiz

all: $(EXEC)

$(EXEC): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC) -ljansson

lib:
	@mkdir -p $(BUILD_DIR)
	@echo "Building jansson-2.14 library"
	@cd $(BUILD_DIR) && cmake -DJANSSON_BUILD_DOCS=OFF ../3rd-party/jansson-2.14/ && make
	@echo "Installing jansson-2.14 library"
	@cd $(BUILD_DIR) && make install

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean lib