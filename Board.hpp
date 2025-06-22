#pragma once

#include <vector>
#include <numeric> // For std::accumulate if needed for printing or IDs
#include <algorithm> // For std::all_of, std::sort, etc.
#include "Tile.hpp"
#include "runs.hpp"     // For isValidRun
#include "groups.hpp"   // For isValidGroup
#include "utilities.hpp" // For sorting tiles if necessary within sets

// Forward declaration if needed, but direct include might be simpler for now
// class Tile;

enum class SetType {
    RUN,
    GROUP
};

class GameSet {
public:
    std::vector<Tile> tiles;
    SetType type;

    GameSet(const std::vector<Tile>& t, SetType st) : tiles(t), type(st) {
        // Ensure tiles are sorted for consistent representation, especially for runs.
        // Groups might not strictly need sorting by number, but color sorting is implicit in Tile comparison.
        // For runs, number sorting is critical.
        std::sort(this->tiles.begin(), this->tiles.end());
    }

    // Basic validation based on type
    bool isValid() const {
        if (tiles.empty()) {
            return false; // An empty set is not valid
        }
        if (type == SetType::RUN) {
            return isValidRun(tiles);
        } else { // SetType::GROUP
            return isValidGroup(tiles);
        }
    }

    // Helper to print a set (optional, for debugging)
    void print() const {
        std::cout << (type == SetType::RUN ? "Run: " : "Group: ");
        for (const auto& tile : tiles) {
            tile.print(); // Assuming Tile has a print method
            std::cout << " ";
        }
        std::cout << (isValid() ? "(Valid)" : "(Invalid)");
        std::cout << std::endl;
    }

    // Equality operator for comparing GameSets - useful for testing
    bool operator==(const GameSet& other) const {
        if (type != other.type || tiles.size() != other.tiles.size()) {
            return false;
        }
        // Assumes tiles are sorted for consistent comparison
        return tiles == other.tiles;
    }

    // Less-than operator for sorting GameSets (e.g., for use in std::set or std::sort)
    bool operator<(const GameSet& other) const {
        if (type != other.type) {
            // Arbitrary but consistent: e.g., RUNs come before GROUPs
            return static_cast<int>(type) < static_cast<int>(other.type);
        }
        // If types are the same, compare based on tiles.
        // std::vector's operator< compares lexicographically.
        // This relies on Tile::operator< and tiles being sorted within GameSet.
        return tiles < other.tiles;
    }
};

class BoardState {
public:
    std::vector<GameSet> sets;

    BoardState() = default;

    BoardState(const std::vector<GameSet>& initial_sets) : sets(initial_sets) {}

    void addSet(const GameSet& set) {
        if (set.isValid()) { // Optionally only add valid sets, or let validation happen elsewhere
            sets.push_back(set);
        }
    }

    // Method to get all tiles currently on the board
    std::vector<Tile> getAllTiles() const {
        std::vector<Tile> all_tiles;
        for (const auto& set : sets) {
            all_tiles.insert(all_tiles.end(), set.tiles.begin(), set.tiles.end());
        }
        // Sorting can make it easier to compare collections of tiles if needed
        std::sort(all_tiles.begin(), all_tiles.end());
        return all_tiles;
    }

    // Helper to print the board state (optional, for debugging)
    void print() const {
        if (sets.empty()) {
            std::cout << "Board is empty." << std::endl;
            return;
        }
        std::cout << "Board State:" << std::endl;
        for (size_t i = 0; i < sets.size(); ++i) {
            std::cout << "Set " << i + 1 << ": ";
            sets[i].print();
        }
    }

    // Equality operator for comparing BoardStates - useful for testing
    bool operator==(const BoardState& other) const {
        if (sets.size() != other.sets.size()) {
            return false;
        }
        // This requires GameSet to have a reliable operator==
        // And assumes the order of sets doesn't matter for equality,
        // which might be true if we sort them or use a more complex check.
        // For now, let's assume order matters or they are sorted before comparison.
        // A more robust comparison would involve sorting copies of the sets vectors
        // or checking for set equivalence regardless of order.
        // For simplicity now:
        return sets == other.sets; // Relies on GameSet::operator== and vector::operator==
    }

    // Checks if all sets on the board are valid and all tiles are unique across sets.
    bool isValidBoard() const {
        if (sets.empty()) {
            return true; // An empty board is considered valid.
        }

        std::set<Tile> tiles_on_board_check; // To check for uniqueness of tiles. Requires Tile::operator<
        size_t total_tiles_in_all_sets = 0;

        for (const auto& game_set : sets) {
            if (!game_set.isValid()) {
                // Optional: Debug print for which set is invalid
                // std::cout << "Debug isValidBoard: Invalid set detected: "; game_set.print();
                return false;
            }
            total_tiles_in_all_sets += game_set.tiles.size();
            for (const auto& tile : game_set.tiles) {
                tiles_on_board_check.insert(tile);
            }
        }

        // If the number of unique tiles inserted into the set is not equal to the
        // total number of tiles counted across all sets, it means a tile was duplicated
        // (used in more than one set).
        if (tiles_on_board_check.size() != total_tiles_in_all_sets) {
             // Optional: Debug print for tile duplication
             // std::cout << "Debug isValidBoard: Tile duplication across sets. Unique: "
             //           << tiles_on_board_check.size() << ", Total in sets: " << total_tiles_in_all_sets << std::endl;
            return false;
        }

        return true;
    }
};
