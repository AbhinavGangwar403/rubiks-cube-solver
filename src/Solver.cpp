#include "Solver.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace {
const std::array<std::string, 18> ALL_MOVES = {
    "U", "U'", "U2", "R", "R'", "R2", "F", "F'", "F2",
    "D", "D'", "D2", "L", "L'", "L2", "B", "B'", "B2"
};

bool sameFace(const std::string& a, const std::string& b) {
    return !a.empty() && !b.empty() && a[0] == b[0];
}

int heuristic(const Cube& cube) {
    const std::string state = cube.toString();
    int misplaced = 0;
    for (int face = 0; face < 6; ++face) {
        const char target = "URFDLB"[face];
        for (int i = 0; i < 9; ++i)
            if (state[face * 9 + i] != target) ++misplaced;
    }
    // A face turn affects at most 20 non-centre stickers; this is a weak,
    // conservative lower bound intended for an educational demonstration.
    return (misplaced + 19) / 20;
}

struct Node {
    Cube cube;
    std::vector<std::string> path;
    int g = 0;
    int h = 0;
};

struct CompareNode {
    bool operator()(const Node& a, const Node& b) const {
        if (a.g + a.h != b.g + b.h) return a.g + a.h > b.g + b.h;
        return a.h > b.h;
    }
};
}

SolveResult Solver::iterativeDeepening(const Cube& start, int maxDepth) {
    SolveResult result;
    if (start.isSolved()) { result.solved = true; result.depth = 0; return result; }

    std::vector<std::string> path;
    std::unordered_set<std::string> onPath;

    for (int limit = 1; limit <= maxDepth; ++limit) {
        onPath.clear();
        onPath.insert(start.toString());

        auto dfs = [&](auto&& self, const Cube& current, int remaining) -> bool {
            ++result.expandedNodes;
            if (current.isSolved()) return true;
            if (remaining == 0) return false;

            for (const auto& move : ALL_MOVES) {
                if (!path.empty() && sameFace(path.back(), move)) continue;
                Cube next = current;
                next.applyMove(move);
                const std::string key = next.toString();
                if (onPath.count(key)) continue;

                path.push_back(move);
                onPath.insert(key);
                if (self(self, next, remaining - 1)) return true;
                onPath.erase(key);
                path.pop_back();
            }
            return false;
        };

        if (dfs(dfs, start, limit)) {
            result.solved = true;
            result.moves = path;
            result.depth = static_cast<int>(path.size());
            return result;
        }
    }
    return result;
}

SolveResult Solver::aStar(const Cube& start, int maxDepth, std::size_t nodeLimit) {
    SolveResult result;
    if (start.isSolved()) { result.solved = true; result.depth = 0; return result; }

    std::priority_queue<Node, std::vector<Node>, CompareNode> open;
    std::unordered_map<std::string, int> bestG;
    open.push(Node{start, {}, 0, heuristic(start)});
    bestG[start.toString()] = 0;

    while (!open.empty() && result.expandedNodes < nodeLimit) {
        Node current = open.top();
        open.pop();

        const std::string currentKey = current.cube.toString();
        auto known = bestG.find(currentKey);
        if (known != bestG.end() && current.g > known->second) continue;

        ++result.expandedNodes;
        if (current.cube.isSolved()) {
            result.solved = true;
            result.moves = current.path;
            result.depth = current.g;
            return result;
        }
        if (current.g >= maxDepth) continue;

        for (const auto& move : ALL_MOVES) {
            if (!current.path.empty() && sameFace(current.path.back(), move)) continue;

            Cube next = current.cube;
            next.applyMove(move);
            const int nextG = current.g + 1;
            const std::string key = next.toString();

            auto it = bestG.find(key);
            if (it != bestG.end() && it->second <= nextG) continue;

            bestG[key] = nextG;
            auto nextPath = current.path;
            nextPath.push_back(move);
            open.push(Node{next, std::move(nextPath), nextG, heuristic(next)});
        }
    }
    return result;
}
