#pragma once

#include <vector>
#include <algorithm> // For std::sort, std::all_of
#include <iostream>  // For GameSet::print
#include <set>       // For GameSet internal checks if any, or if used by isValid functions

#include "Tile.hpp"
#include "runs.hpp"   // For isValidRun
#include "groups.hpp" // For isValidGroup
#include "PerformanceTracer.hpp" // For performance tracing
// utilities.hpp might be needed if GameSet used functions from it, currently doesn't seem to.
// However, isValidRun/isValidGroup from runs.hpp/groups.hpp *do* include utilities.hpp.

enum class SetType {
    RUN,
    GROUP
};

class GameSet {
public:
    std::vector<Tile> tiles;
    SetType type;

    GameSet(const std::vector<Tile>& t, SetType st) : tiles(t), type(st) {
        std::sort(this->tiles.begin(), this->tiles.end());
    }

    bool isValid() const {
        TRACE_FUNCTION();
        if (tiles.empty()) {
            return false;
        }
        if (type == SetType::RUN) {
            return isValidRun(tiles);
        } else { // SetType::GROUP
            return isValidGroup(tiles);
        }
    }

    void print() const {
        std::cout << (type == SetType::RUN ? "Run: " : "Group: ");
        for (const auto& tile : tiles) {
            tile.print();
            std::cout << " ";
        }
        std::cout << (isValid() ? "(Valid)" : "(Invalid)");
        std::cout << std::endl;
    }

    bool operator==(const GameSet& other) const {
        if (type != other.type || tiles.size() != other.tiles.size()) {
            return false;
        }
        return tiles == other.tiles; // Relies on Tile::operator== and sorted tiles
    }

    bool operator<(const GameSet& other) const {
        if (type != other.type) {
            return static_cast<int>(type) < static_cast<int>(other.type);
        }
        return tiles < other.tiles; // Relies on Tile::operator< and sorted tiles
    }
};
