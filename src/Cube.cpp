#include "Cube.hpp"

#include <cctype>
#include <iostream>
#include <stdexcept>

Cube::Cube() : Cube(Cube::solved().state()) {}

Cube::Cube(const State& state) : stickers_(state) {}

Cube Cube::solved() {
    State state{};
    const char faces[6] = {'U', 'R', 'F', 'D', 'L', 'B'};
    for (int face = 0; face < 6; ++face) {
        for (int i = 0; i < 9; ++i) state[face * 9 + i] = faces[face];
    }
    return Cube(state);
}

bool Cube::isValidFace(char face) {
    return face == 'U' || face == 'R' || face == 'F' ||
           face == 'D' || face == 'L' || face == 'B';
}

int Cube::faceIndex(char face) {
    switch (face) {
        case 'U': return 0;
        case 'R': return 1;
        case 'F': return 2;
        case 'D': return 3;
        case 'L': return 4;
        case 'B': return 5;
        default: throw std::invalid_argument("Unknown face: " + std::string(1, face));
    }
}

Cube::Vec3 Cube::positionForIndex(int index) {
    const int face = index / 9;
    const int cell = index % 9;
    const int row = cell / 3;
    const int col = cell % 3;

    switch (face) {
        case 0: return {col - 1, 1, row - 1};       // U
        case 1: return {1, 1 - row, 1 - col};       // R
        case 2: return {col - 1, 1 - row, 1};       // F
        case 3: return {col - 1, -1, 1 - row};      // D
        case 4: return {-1, 1 - row, col - 1};      // L
        case 5: return {1 - col, 1 - row, -1};      // B
        default: throw std::out_of_range("Sticker index out of range");
    }
}

Cube::Vec3 Cube::normalForIndex(int index) {
    switch (index / 9) {
        case 0: return {0, 1, 0};
        case 1: return {1, 0, 0};
        case 2: return {0, 0, 1};
        case 3: return {0, -1, 0};
        case 4: return {-1, 0, 0};
        case 5: return {0, 0, -1};
        default: throw std::out_of_range("Sticker index out of range");
    }
}

int Cube::indexForPositionAndNormal(const Vec3& p, const Vec3& n) {
    for (int i = 0; i < StickerCount; ++i) {
        const Vec3 q = positionForIndex(i);
        const Vec3 m = normalForIndex(i);
        if (q.x == p.x && q.y == p.y && q.z == p.z &&
            m.x == n.x && m.y == n.y && m.z == n.z) return i;
    }
    throw std::logic_error("Could not map rotated sticker to a facelet");
}

Cube::Vec3 Cube::rotateQuarterTurn(Vec3 v, Vec3 axis, int direction) {
    // direction +1 is +90 degrees by the right-hand rule; -1 is -90 degrees.
    const int dot = axis.x * v.x + axis.y * v.y + axis.z * v.z;
    const Vec3 parallel{axis.x * dot, axis.y * dot, axis.z * dot};
    const Vec3 cross{
        axis.y * v.z - axis.z * v.y,
        axis.z * v.x - axis.x * v.z,
        axis.x * v.y - axis.y * v.x
    };
    return {
        parallel.x + direction * cross.x,
        parallel.y + direction * cross.y,
        parallel.z + direction * cross.z
    };
}

void Cube::applyMove(const std::string& move) {
    if (move.empty() || !isValidFace(move[0])) {
        throw std::invalid_argument("Invalid move '" + move + "'. Use U, R, F, D, L, B with optional ' or 2.");
    }

    int turns = 1;
    if (move.size() == 2 && move[1] == '\'') turns = 3;
    else if (move.size() == 2 && move[1] == '2') turns = 2;
    else if (move.size() != 1) {
        throw std::invalid_argument("Invalid move suffix in '" + move + "'");
    }

    const char face = move[0];
    const int fi = faceIndex(face);
    const Vec3 axis = normalForIndex(fi * 9);
    State next = stickers_;

    for (int i = 0; i < StickerCount; ++i) {
        Vec3 p = positionForIndex(i);
        Vec3 n = normalForIndex(i);
        const int layer = p.x * axis.x + p.y * axis.y + p.z * axis.z;
        if (layer != 1) continue;

        for (int t = 0; t < turns; ++t) {
            p = rotateQuarterTurn(p, axis, -1);
            n = rotateQuarterTurn(n, axis, -1);
        }
        const int destination = indexForPositionAndNormal(p, n);
        next[destination] = stickers_[i];
    }
    stickers_ = next;
}

void Cube::applyMoves(const std::vector<std::string>& moves) {
    for (const auto& move : moves) applyMove(move);
}

std::vector<std::string> Cube::parseMoves(const std::string& notation) {
    std::vector<std::string> moves;
    std::string token;
    for (char ch : notation) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!token.empty()) { moves.push_back(token); token.clear(); }
        } else {
            token += ch;
        }
    }
    if (!token.empty()) moves.push_back(token);
    for (const auto& move : moves) {
        if (move.empty() || !isValidFace(move[0]) ||
            (move.size() > 2) ||
            (move.size() == 2 && move[1] != '\'' && move[1] != '2')) {
            throw std::invalid_argument("Invalid move token: " + move);
        }
    }
    return moves;
}

bool Cube::isSolved() const {
    for (int face = 0; face < 6; ++face) {
        const char color = stickers_[face * 9];
        for (int i = 1; i < 9; ++i) {
            if (stickers_[face * 9 + i] != color) return false;
        }
    }
    return true;
}

std::string Cube::toString() const {
    return std::string(stickers_.begin(), stickers_.end());
}

void Cube::printNet() const {
    auto printFace = [this](int face, int indent) {
        for (int row = 0; row < 3; ++row) {
            std::cout << std::string(indent, ' ');
            for (int col = 0; col < 3; ++col) {
                std::cout << stickers_[face * 9 + row * 3 + col] << ' ';
            }
            std::cout << '\n';
        }
    };
    std::cout << "        U\n";
    printFace(0, 6);
    std::cout << "L       F       R       B\n";
    for (int row = 0; row < 3; ++row) {
        for (int face : {4, 2, 1, 5}) {
            for (int col = 0; col < 3; ++col)
                std::cout << stickers_[face * 9 + row * 3 + col] << ' ';
            std::cout << "  ";
        }
        std::cout << '\n';
    }
    std::cout << "        D\n";
    printFace(3, 6);
}
