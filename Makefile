# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Source files and object files
SRCS = main.cpp dijkstra.cpp
MICROBENCH_SRCS = microbench.cpp dijkstra.cpp
OBJ_DIR = objects
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
MICROBENCH_OBJS = $(MICROBENCH_SRCS:%.cpp=$(OBJ_DIR)/%.o)

# Output binaries
TARGET = program
MICROBENCH_TARGET = microbench

# Default rule: build both targets
all: $(TARGET) $(MICROBENCH_TARGET)

# Rule to link object files into the main executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Rule to link object files into the microbench executable
$(MICROBENCH_TARGET): $(OBJ_DIR)/microbench.o $(OBJ_DIR)/dijkstra.o
	$(CXX) $(OBJ_DIR)/microbench.o $(OBJ_DIR)/dijkstra.o -o $(MICROBENCH_TARGET)

# Rule to compile .cpp files into .o files
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(OBJ_DIR)  # Create the objects directory if it doesn't exist
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(TARGET) $(MICROBENCH_TARGET)

# Phony targets
.PHONY: all clean
