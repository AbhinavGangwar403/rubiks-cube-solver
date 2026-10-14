# Rubik's Cube Solver in C++

A command-line, educational 3×3 Rubik's Cube solver demonstrating Object-Oriented Programming, state-space search, Iterative Deepening Depth-First Search (IDDFS), and A* search.

## Features

- Sticker-based 3×3 cube state (54 facelets).
- Face-turn notation: `U`, `R`, `F`, `D`, `L`, `B`, with inverse (`'`) and half-turn (`2`) suffixes.
- Two search strategies: IDDFS and A*.
- Configurable maximum solution depth.
- Cube-net display before and after solving.
- Verifies the returned sequence by applying it to the cube.
- CMake build setup; no third-party libraries required.

## Project structure

```text
rubiks-cube-solver-cpp/
├── include/
│   ├── Cube.hpp
│   └── Solver.hpp
├── src/
│   ├── Cube.cpp
│   ├── Solver.cpp
│   └── main.cpp
├── examples/
│   └── scrambles.txt
├── CMakeLists.txt
├── .gitignore
└── README.md
```

## Requirements

- A C++17 compiler (GCC, Clang, or MSVC)
- CMake 3.16+

## Build

### Windows (PowerShell / Developer Command Prompt)

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Run:

```powershell
.\build\Release\rubiks_solver.exe iddfs 6 "R U R' U'"
```

Depending on your generator, the executable may be at `build\rubiks_solver.exe` instead.

### Linux / macOS

```bash
cmake -S . -B build
cmake --build build -j
./build/rubiks_solver iddfs 6 "R U R' U'"
```

## Usage

```text
rubiks_solver <iddfs|astar> <max-depth> "<scramble>"
```

Examples:

```bash
./build/rubiks_solver iddfs 6 "R U R' U'"
./build/rubiks_solver astar 7 "F R U R' U' F'"
./build/rubiks_solver iddfs 0 ""
```

Moves are separated by spaces. Standard face notation is used:
- `R` = right face clockwise
- `R'` = right face counter-clockwise
- `R2` = right face half-turn
- The same suffix rules apply to `U`, `F`, `D`, `L`, and `B`.

The program begins with a solved cube, applies the supplied scramble to create the initial state, then searches for a path back to the solved target state.

## Algorithms

### Iterative Deepening DFS (IDDFS)

Runs depth-limited DFS repeatedly, increasing the limit from 1 to the configured maximum. It uses little memory compared with breadth-first search, but runtime grows quickly as the depth increases.

### A* search

Uses `f(n) = g(n) + h(n)`, where `g(n)` is the number of moves so far and `h(n)` is a conservative sticker-mismatch lower bound. This heuristic is intentionally simple for demonstrating A*, not a competitive Rubik's Cube heuristic.

## Important limitations

This is an educational search project, not a speed-optimized competition solver. The search is bounded by the selected depth; A* also has a 500,000-node expansion limit. It is best for short scrambles. Longer scrambles may not be solved within the configured limits. A production-grade solver typically uses cubie coordinates and stronger pattern-database or two-phase heuristics.

The input scramble is assumed to describe a legal cube state. This version does not validate arbitrary manually entered 54-sticker states.

## Example scrambles

See [`examples/scrambles.txt`](examples/scrambles.txt).

## GitHub

```bash
git init
git add .
git commit -m "Add C++ Rubik's Cube solver with IDDFS and A*"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/rubiks-cube-solver-cpp.git
git push -u origin main
```

Replace `YOUR_USERNAME` with your GitHub username. If the repository already exists and has a remote configured, do not add the remote again.

## Possible future improvements

- Accept and validate a user-entered cube state.
- Add a scramble generator.
- Improve the heuristic with corner/edge pattern databases.
- Add unit tests for move inverses and four-turn identity.
- Add an interactive mode and graphical visualization.
