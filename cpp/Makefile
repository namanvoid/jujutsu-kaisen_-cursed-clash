CXX ?= clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude -I/opt/homebrew/include -I/usr/local/include

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
endif

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = ../bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/cursed_clash

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	/bin/mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	/bin/mkdir -p $(BIN_DIR)

clean:
	/bin/rm -rf $(OBJ_DIR) $(TARGET)

run: $(TARGET)
	$(TARGET)

test: $(TARGET)
	$(TARGET) --test

.PHONY: all clean run test
