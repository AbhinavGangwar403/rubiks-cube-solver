#pragma once

#include "Cube.hpp"
#include <string>
#include <vector>

struct SolveResult {
    bool solved = false;
    std::vector<std::string> moves;
    std::size_t expandedNodes = 0;
    int depth = -1;
};

class Solver {
public:
    // Educational search solvers. Depth is deliberately bounded.
    static SolveResult iterativeDeepening(const Cube& start, int maxDepth);
    static SolveResult aStar(const Cube& start, int maxDepth, std::size_t nodeLimit = 500000);
};
