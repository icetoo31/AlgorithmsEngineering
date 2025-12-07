# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Source files and object files
SRCS = main.cpp dijkstra.cpp
OBJ_DIR = objects
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Output binary
TARGET = program

# Default rule: build the target
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Rule to compile .cpp files into .o files
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)  # Create the objects directory if it doesn't exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
