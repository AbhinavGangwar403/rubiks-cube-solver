#pragma once

#include <array>
#include <string>
#include <vector>

class Cube {
public:
    static constexpr int StickerCount = 54;
    using State = std::array<char, StickerCount>;

    Cube();
    explicit Cube(const State& state);

    static Cube solved();
    static bool isValidFace(char face);
    static std::vector<std::string> parseMoves(const std::string& notation);

    void applyMove(const std::string& move);
    void applyMoves(const std::vector<std::string>& moves);
    bool isSolved() const;
    std::string toString() const;
    void printNet() const;

    const State& state() const { return stickers_; }

    bool operator==(const Cube& other) const { return stickers_ == other.stickers_; }
    bool operator!=(const Cube& other) const { return !(*this == other); }

public:
    struct Vec3 { int x, y, z; };

private:
    State stickers_{};
    static Vec3 positionForIndex(int index);
    static Vec3 normalForIndex(int index);
    static int indexForPositionAndNormal(const Vec3& position, const Vec3& normal);
    static Vec3 rotateQuarterTurn(Vec3 v, Vec3 axis, int direction);
    static int faceIndex(char face);
};
