# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++23 -Wall -I./src -I./dummy

# Target executable name
TARGET   := main

SRCS     := main.cpp $(wildcard src/*.cpp) $(wildcard dummy/*.cpp)
OBJS     := $(SRCS:.cpp=.o)

# Default target: build the executable
all: $(TARGET)

# Link object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Pattern rule: compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Declare non-file targets
.PHONY: all clean
