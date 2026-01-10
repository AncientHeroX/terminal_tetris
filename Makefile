CC=clang
APPNAME=tetris

LIB_DIR=./lib
INCLUDE_DIR=./include
BIN_DIR=./bin/debug
BUILD_DIR=./build/debug
SRC_DIR=./src

LIBS=-lm -lncurses
CFLAGS=-I$(INCLUDE_DIR) -g -Wall
LDFLAGS= -fsanitize=address
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

SRCS = $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.c.o,$(SRCS))


$(BIN_DIR)/$(APPNAME): $(OBJS) $(BUILD_DIR)/miniaudio.o
	mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/miniaudio.o : $(INCLUDE_DIR)/miniaudio.h
	$(CC) -DMINIAUDIO_IMPLEMENTATION -x c -c $^ -o $@

$(BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS) -DDEBUG

R_BUILD_DIR=./build/release
R_BIN_DIR=./bin/release
R_SRCS = $(shell find $(SRC_DIR) -name '*.c')
R_OBJS := $(patsubst $(SRC_DIR)/%.c,$(R_BUILD_DIR)/%.c.o,$(SRCS))
R_CFLAGS=-I$(INCLUDE_DIR) -O2 

$(R_BIN_DIR)/$(APPNAME): $(R_OBJS) $(R_BUILD_DIR)/miniaudio.o
	mkdir -p $(R_BIN_DIR)
	$(CC) -o $@ $^ $(R_LDFLAGS) $(LIBS)

$(R_BUILD_DIR)/miniaudio.o : $(INCLUDE_DIR)/miniaudio.h
	$(CC) -DMINIAUDIO_IMPLEMENTATION -x c -c $^ -o $@

$(R_BUILD_DIR)/%.c.o: $(SRC_DIR)/%.c
	mkdir -p $(R_BUILD_DIR)
	$(CC) -c $< -o $@ $(R_CFLAGS) -DNDEBUG
	
release: $(R_BIN_DIR)/$(APPNAME)

.PHONY: clean
clean: 
	rm -rf bin/* build/*
