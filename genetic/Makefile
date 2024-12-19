# Compiler settings
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Source files and objects
SRCS = main.cpp utils.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = genetic_solver

# Debug flags
DEBUG_FLAGS = -g -DDEBUG

# Instance settings
INSTANCE_NUM ?= 01
INSTANCE_PATH = ../instances/$(INSTANCE_NUM).txt

# Targets
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: all

clean:
	rm -f $(OBJS) $(TARGET)

# Validate instance exists
check_instance:
	@test -f $(INSTANCE_PATH) || (echo "Error: Instance file $(INSTANCE_PATH) not found" && exit 1)

# Run target with instance
run: all check_instance
	./$(TARGET) $(INSTANCE_PATH)

.PHONY: all clean debug

# Dependencies
main.o: main.cpp utils.h
utils.o: utils.cpp utils.h
