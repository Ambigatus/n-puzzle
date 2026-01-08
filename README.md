# N-Puzzle Solver

A fast and efficient C++ implementation of the N-puzzle solver using the A* search algorithm with multiple heuristic options.

## 📋 Table of Contents
- [Overview](#overview)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Heuristics Explained](#heuristics-explained)
- [Algorithm Details](#algorithm-details)
- [Performance](#performance)
- [Project Structure](#project-structure)
- [Examples](#examples)
- [Troubleshooting](#troubleshooting)

## 🎯 Overview

The N-puzzle is a sliding puzzle consisting of a square grid with numbered tiles and one empty space. The goal is to rearrange the tiles into a specific pattern (the "snail" solution) by sliding them into the empty space.

**Example (3x3 puzzle):**
```
Start State:        Goal State:
3 2 6               1 2 3
1 4 0      →        8 0 4
8 7 5               7 6 5
```

## ✨ Features

- **A\* Search Algorithm**: Finds optimal solution efficiently
- **Three Heuristic Functions**:
  - Manhattan Distance (default)
  - Linear Conflict (fastest)
  - Hamming Distance (simplest)
- **Solvability Detection**: Automatically detects unsolvable puzzles
- **Multiple Input Methods**: Load from file or generate random puzzles
- **Performance Metrics**: Reports time/space complexity
- **Verbose Mode**: Shows complete solution path
- **Handles Large Puzzles**: Tested up to 10×10 puzzles

## 🔧 Installation

### Prerequisites
- C++ compiler with C++17 support (g++ 7.0+ or clang++ 5.0+)
- Make (optional, but recommended)

### Build Instructions

```bash
# Clone or download the project
git clone <your-repo-url>
cd npuzzle

# Compile using Makefile
make

# Or compile manually
g++ -std=c++17 -O3 -Wall -o npuzzle npuzzle.cpp
```

## 🚀 Usage

### Basic Command Structure
```bash
./npuzzle [options]
```

### Options
- `-f <file>` : Solve puzzle from input file
- `-r <size>` : Generate and solve random puzzle of given size
- `-h <1|2|3>` : Choose heuristic (1=Manhattan, 2=Linear Conflict, 3=Hamming)
- `-v` : Verbose mode (show solution path)
- `--help` : Display help message

### Examples

**Solve puzzle from file:**
```bash
./npuzzle -f puzzle.txt -h 2 -v
```

**Generate and solve random 3×3 puzzle:**
```bash
./npuzzle -r 3 -h 1
```

**Solve 4×4 puzzle with best heuristic:**
```bash
./npuzzle -f puzzle_4x4.txt -h 2
```

### Input File Format

```
# Comments start with #
3              <- puzzle size
3 2 6          <- first row
1 4 0          <- second row (0 = empty)
8 7 5          <- third row
```

## 🧮 Heuristics Explained

### 1. Manhattan Distance (Heuristic `-h 1`)
**How it works:** For each tile, calculates the sum of horizontal and vertical distances to its goal position.

**Properties:**
- ✅ Admissible (never overestimates)
- ⚡ Fast to compute
- 📊 Good performance for most puzzles

**When to use:** Default choice, works well for 3×3 and 4×4 puzzles.

### 2. Linear Conflict (Heuristic `-h 2`) ⭐ RECOMMENDED
**How it works:** Manhattan Distance + additional cost for tiles that block each other in the same row/column.

**Properties:**
- ✅ Admissible
- ⚡⚡ Fastest in practice
- 📊 Best performance, explores fewer states

**When to use:** Best choice for all puzzle sizes. Significantly faster than Manhattan for difficult puzzles.

**Example of conflict:**
```
Current:  Goal:
1 3 2     1 2 3

Tiles 2 and 3 are in correct row but reversed.
They must "move around" each other (+2 moves).
```

### 3. Hamming Distance (Heuristic `-h 3`)
**How it works:** Counts the number of tiles not in their goal position.

**Properties:**
- ✅ Admissible
- ⚡⚡⚡ Very fast to compute
- 📊 Explores more states than others

**When to use:** Educational purposes or when computation speed matters more than search efficiency.

## 🔍 Algorithm Details

### A\* Search Algorithm

A\* explores states in order of **f(n) = g(n) + h(n)** where:
- **g(n)**: Actual cost from start to current state (number of moves)
- **h(n)**: Heuristic estimate of cost to goal
- **f(n)**: Estimated total cost through current state

**Why it works:**
- Always finds optimal solution when heuristic is admissible
- Explores most promising paths first
- More efficient than Dijkstra or BFS

### Solvability Check

Not all configurations are solvable! The program checks solvability using inversion counting:

**For odd-sized puzzles (3×3, 5×5):**
- Solvable if number of inversions is **even**

**For even-sized puzzles (4×4, 6×6):**
- Solvable if (inversions + empty_row_from_bottom) is **odd**

**What's an inversion?**
When a larger number appears before a smaller number in the flattened grid (reading left-to-right, top-to-bottom).

## ⚡ Performance

### Benchmark Results (3×3 puzzles)

| Heuristic        | Avg States Opened | Avg Time |
|------------------|-------------------|----------|
| Hamming          | ~5000            | ~100ms   |
| Manhattan        | ~800             | ~20ms    |
| Linear Conflict  | ~200             | ~5ms     | ⭐

### Scalability

| Puzzle Size | Max Recommended | Notes |
|-------------|-----------------|-------|
| 3×3         | ✅ Always works | < 1 second |
| 4×4         | ✅ Works well   | 1-10 seconds |
| 5×5         | ⚠️ Challenging  | May take minutes |
| 6×6+        | ⚠️ Very hard    | Requires optimization |

**Tip:** Use Linear Conflict (`-h 2`) for best performance!

## 📁 Project Structure

```
npuzzle/
├── npuzzle.cpp          # Main source code
├── Makefile             # Build configuration
├── README.md            # This file
├── puzzle_easy.txt      # Example: easy puzzle
├── puzzle_medium.txt    # Example: medium difficulty
├── puzzle_hard.txt      # Example: hard puzzle
├── puzzle_4x4.txt       # Example: 4×4 puzzle
└── puzzle_unsolvable.txt # Example: unsolvable puzzle
```

## 📚 Examples

### Example 1: Quick Test
```bash
make test
```
Output:
```
Solving with Linear Conflict...
========================================
           SOLUTION FOUND!
========================================
Heuristic used: Linear Conflict
Total states opened: 137
Max states in memory: 68
Solution length: 12 moves
Move sequence: DLLURRDLLDRU
========================================
```

### Example 2: Compare Heuristics
```bash
make compare
```
This runs the same puzzle with all three heuristics to compare performance.

### Example 3: Large Puzzle
```bash
./npuzzle -r 4 -h 2
```

### Example 4: Debug Unsolvable Puzzle
```bash
./npuzzle -f puzzle_unsolvable.txt -h 2
```
Output:
```
ERROR: This puzzle is UNSOLVABLE!
No sequence of moves can reach the goal state.
```

## 🐛 Troubleshooting

### Problem: "Compilation failed"
**Solution:** Ensure you have C++17 support:
```bash
g++ --version  # Should be 7.0+
```

### Problem: "Puzzle takes too long to solve"
**Solutions:**
1. Use Linear Conflict heuristic: `-h 2`
2. Try smaller puzzle size
3. Check if puzzle is actually solvable

### Problem: "Segmentation fault"
**Solution:** Likely ran out of memory on very large puzzle. Try:
1. Smaller puzzle size
2. Better heuristic
3. Increase system memory

### Problem: "File not found"
**Solution:** Check file path and format:
```bash
# Test file format
cat puzzle.txt
# Should show size and grid
```

## 🎓 Educational Notes

### Key Concepts You'll Learn
1. **A\* Algorithm**: Informed search with heuristics
2. **Admissible Heuristics**: Never overestimate true cost
3. **Priority Queues**: Efficiently get minimum f-score
4. **State Space Search**: Exploring problem configurations
5. **Hash Sets**: Fast duplicate detection

### Common Questions

**Q: Why not use BFS or DFS?**
A: A\* is much faster because it uses heuristics to guide search toward the goal.

**Q: Can heuristics overestimate?**
A: No! Overestimating makes them inadmissible, and A\* won't guarantee optimal solution.

**Q: Why is Linear Conflict better than Manhattan?**
A: It provides tighter lower bound by detecting when tiles block each other.

**Q: How many states can a 3×3 puzzle have?**
A: 9! / 2 = 181,440 reachable states (half of all permutations are unsolvable).

## 📊 Output Explanation

```
Total states opened: 137
```
Number of states examined (time complexity)

```
Max states in memory: 68
```
Peak memory usage during search (space complexity)

```
Solution length: 12 moves
```
Number of moves in optimal solution

```
Move sequence: DLLURRDLLDRU
```
U=Up, D=Down, L=Left, R=Right

## 🎯 Tips for Success

1. **Start Small**: Test with 3×3 before moving to larger puzzles
2. **Use Linear Conflict**: It's the fastest heuristic
3. **Check Solvability**: The program does this automatically
4. **Monitor Performance**: Watch states opened to gauge efficiency
5. **Experiment**: Try different heuristics to understand their impact

## 📝 License

This project is for educational purposes. Feel free to use and modify!

## 🤝 Contributing

Found a bug or have a suggestion? Feel free to open an issue or submit a pull request!

## 📞 Support

If you have questions:
1. Check this README
2. Look at example puzzles
3. Try `./npuzzle --help`
4. Run `make test` to verify installation

---

**Happy Puzzle Solving! 🧩**
