#include "Cube.hpp"
#include "Solver.hpp"

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

void printUsage(const char* program) {
    std::cout << "Rubik's Cube Solver (educational 3x3 sticker model)\n\n"
              << "Usage:\n  " << program << " <iddfs|astar> <max-depth> <scramble moves>\n\n"
              << "Examples:\n"
              << "  " << program << " iddfs 6 \"R U R' U'\"\n"
              << "  " << program << " astar 7 \"F R U R' U' F'\"\n\n"
              << "Move notation: U R F D L B; append ' for inverse or 2 for a half-turn.\n"
              << "The solver starts from the solved cube, applies the scramble, then searches\n"
              << "for a sequence that returns the cube to the solved state.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage(argv[0]);
        return 1;
    }

    try {
        const std::string algorithm = argv[1];
        const int maxDepth = std::stoi(argv[2]);
        if (maxDepth < 0 || maxDepth > 12) {
            std::cerr << "Choose a max depth between 0 and 12.\n";
            return 1;
        }

        std::string notation;
        for (int i = 3; i < argc; ++i) {
            if (!notation.empty()) notation += ' ';
            notation += argv[i];
        }

        const auto scramble = Cube::parseMoves(notation);
        Cube cube = Cube::solved();
        cube.applyMoves(scramble);

        std::cout << "Scramble: " << notation << "\n\nInitial cube:\n";
        cube.printNet();

        SolveResult result;
        if (algorithm == "iddfs") {
            result = Solver::iterativeDeepening(cube, maxDepth);
        } else if (algorithm == "astar") {
            result = Solver::aStar(cube, maxDepth);
        } else {
            std::cerr << "Unknown algorithm. Use 'iddfs' or 'astar'.\n";
            return 1;
        }

        std::cout << "\nAlgorithm: " << algorithm << "\n"
                  << "Expanded nodes: " << result.expandedNodes << "\n";

        if (!result.solved) {
            std::cout << "No solution found within depth " << maxDepth
                      << " (or the A* node limit was reached).\n";
            return 2;
        }

        std::cout << "Solution (" << result.depth << " moves): ";
        if (result.moves.empty()) std::cout << "(already solved)";
        for (const auto& move : result.moves) std::cout << move << ' ';
        std::cout << "\n";

        cube.applyMoves(result.moves);
        std::cout << "\nCube after applying solution:\n";
        cube.printNet();
        std::cout << "\nVerification: " << (cube.isSolved() ? "SOLVED" : "FAILED") << '\n';
        return cube.isSolved() ? 0 : 3;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
