# ============================================================================
# N-PUZZLE MAKEFILE
# ============================================================================

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -pedantic
DEBUG_FLAGS = -std=c++17 -g -Wall -Wextra -pedantic -DDEBUG

# Target executable name
TARGET = npuzzle
DEBUG_TARGET = npuzzle_debug

# Source files
SRCS = npuzzle.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# ============================================================================
# RULES
# ============================================================================

# Default target: build optimized version
all: $(TARGET)

# Build optimized executable
$(TARGET): $(SRCS)
	@echo "Compiling optimized version..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)
	@echo "Build complete: $(TARGET)"
	@echo ""
	@echo "Usage examples:"
	@echo "  ./$(TARGET) -f puzzle.txt -h 2 -v"
	@echo "  ./$(TARGET) -r 3 -h 1"
	@echo ""

# Build debug version with debugging symbols
debug: $(SRCS)
	@echo "Compiling debug version..."
	$(CXX) $(DEBUG_FLAGS) -o $(DEBUG_TARGET) $(SRCS)
	@echo "Debug build complete: $(DEBUG_TARGET)"

# Clean build artifacts
clean:
	@echo "Cleaning build files..."
	rm -f $(TARGET) $(DEBUG_TARGET) $(OBJS)
	@echo "Clean complete"

# Clean and rebuild
re: clean all

# Full clean and rebuild debug version
redebug: clean debug

# Run with example 3x3 puzzle (requires puzzle.txt)
test: $(TARGET)
	@echo "Running test with 3x3 puzzle..."
	@echo "Creating test puzzle file..."
	@echo "# Test 3x3 puzzle" > puzzle.txt
	@echo "3" >> puzzle.txt
	@echo "3 2 6" >> puzzle.txt
	@echo "1 4 0" >> puzzle.txt
	@echo "8 7 5" >> puzzle.txt
	./$(TARGET) -f puzzle.txt -h 2 -v

# Run with random 3x3 puzzle
test_random: $(TARGET)
	@echo "Running test with random 3x3 puzzle..."
	./$(TARGET) -r 3 -h 2 -v

# Run with random 4x4 puzzle (harder)
test_4x4: $(TARGET)
	@echo "Running test with random 4x4 puzzle..."
	./$(TARGET) -r 4 -h 2

# Compare all heuristics on same random puzzle
compare: $(TARGET)
	@echo "Comparing heuristics on 3x3 puzzle..."
	@echo ""
	@echo "=== Manhattan Distance ==="
	./$(TARGET) -r 3 -h 1
	@echo ""
	@echo "=== Linear Conflict ==="
	./$(TARGET) -r 3 -h 2
	@echo ""
	@echo "=== Hamming Distance ==="
	./$(TARGET) -r 3 -h 3

# Install (copy to /usr/local/bin)
install: $(TARGET)
	@echo "Installing $(TARGET)..."
	sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete"

# Uninstall
uninstall:
	@echo "Uninstalling $(TARGET)..."
	sudo rm -f /usr/local/bin/$(TARGET)
	@echo "Uninstall complete"

# Show help
help:
	@echo "N-Puzzle Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build optimized executable (default)"
	@echo "  debug        - Build debug version with symbols"
	@echo "  clean        - Remove build artifacts"
	@echo "  re           - Clean and rebuild"
	@echo "  redebug      - Clean and rebuild debug version"
	@echo "  test         - Run test with example puzzle"
	@echo "  test_random  - Run test with random 3x3 puzzle"
	@echo "  test_4x4     - Run test with random 4x4 puzzle"
	@echo "  compare      - Compare all heuristics"
	@echo "  install      - Install to /usr/local/bin"
	@echo "  uninstall    - Remove from /usr/local/bin"
	@echo "  help         - Show this help"
	@echo ""
	@echo "Program usage:"
	@echo "  ./$(TARGET) -f <file> -h <1|2|3> [-v]"
	@echo "  ./$(TARGET) -r <size> -h <1|2|3> [-v]"
	@echo ""
	@echo "Heuristics:"
	@echo "  1 - Manhattan Distance"
	@echo "  2 - Linear Conflict (recommended)"
	@echo "  3 - Hamming Distance"

# Phony targets (not actual files)
.PHONY: all debug clean re redebug test test_random test_4x4 compare install uninstall help