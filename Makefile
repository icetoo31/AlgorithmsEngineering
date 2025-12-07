# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Source files and object files
SRCS = main.cpp dijkstra.cpp
OBJS = $(SRCS:.cpp=.o)

# Output binary
TARGET = program

# Default rule: build the target
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
