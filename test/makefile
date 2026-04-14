CC      = gcc
CFLAGS  = -Wall -Wextra -fsanitize=address -g
SRC_DIR = ../src
LIBS    = -lsqlite3

# All src files except main.c — it has its own main()
SRCS = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))

test: test.c $(SRCS)
	$(CC) $(CFLAGS) -I$(SRC_DIR) test.c $(SRCS) $(LIBS) -o test_runner

run: test
	./test_runner

clean:
	rm -f test_runner 
	rm -rf test_runner.dSYM

.PHONY: run clean
