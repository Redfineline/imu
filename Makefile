# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2

# Source files
SRC = main.cpp imu_parser.cpp
OBJ = $(SRC:.cpp=.o)

# Output binary
TARGET = imu_parser

# Default rule
all: $(TARGET)

# Link object files to create binary
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile .cpp to .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets (not real files)
.PHONY: all clean

