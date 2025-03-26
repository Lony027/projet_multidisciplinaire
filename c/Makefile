CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR = src
CODE_DIR = code

SRC_FILES = $(wildcard $(CODE_DIR)/*.c)
OBJ_FILES = $(patsubst $(CODE_DIR)/%.c, %.o, $(SRC_FILES))

MAIN_SRC = main.c
MAIN_OBJ = main.o

TARGET = main

all: $(TARGET)
	$(RM) *.o

$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -I. -c $< -o $@

$(TARGET): $(OBJ_FILES) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: $(CODE_DIR)/%.c
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean:
	$(RM) *.o $(TARGET)

rebuild: clean all

.PHONY: all clean rebuild