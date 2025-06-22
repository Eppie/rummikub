#pragma once

#include <vector>
#include <numeric> // For std::accumulate if needed for printing or IDs
#include <algorithm> // For std::all_of, std::sort, etc.
// Tile.hpp, runs.hpp, groups.hpp are now included via GameTypes.hpp
#include "GameTypes.hpp" // Defines GameSet, SetType
#include "utilities.hpp" // For sorting tiles if necessary within sets, and other board utilities
#include "SetFinder.hpp" // For find_all_possible_sets
#include "PerformanceTracer.hpp" // For performance tracing

// class Tile; // Forward declaration no longer needed if GameTypes pulls it.


class BoardState {
public:
    std::vector<GameSet> sets; // GameSet is now from GameTypes.hpp

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
        TRACE_FUNCTION();
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
}; // End of BoardState class definition

// Function to check if a collection of sets represents a valid board state.
// This is useful for validating potential new board configurations before committing them.
inline bool is_board_valid(const std::vector<GameSet>& board_sets) {
    TRACE_FUNCTION();
    if (board_sets.empty()) {
        return true; // An empty collection of sets is valid.
    }
    // Use a temporary BoardState to leverage its isValidBoard logic.
    BoardState temp_board_state(board_sets);
    return temp_board_state.isValidBoard();
}

// Helper namespace for can_add_tiles_to_board logic
namespace BoardManipulation {

// Recursive helper function for can_add_tiles_to_board
// Returns true if a valid arrangement is found, false otherwise.
// result_sets will contain the valid arrangement if true is returned.
bool find_valid_arrangement_recursive(
    std::vector<Tile>& current_pool_tiles, // Tiles remaining to be placed
    const std::vector<GameSet>& all_possible_valid_sets, // All valid sets that can be formed from the initial combined pool
    std::vector<GameSet>& current_arrangement, // The sets formed so far in this path
    const std::set<Tile>& original_tiles_to_add_set, // For checking if all *added* tiles are used
    std::set<Tile>& used_tiles_from_add_pool, // Tracks which of original_tiles_to_add_set are in current_arrangement
    size_t total_tiles_to_place // The total number of tiles in the initial combined pool
) {
    TRACE_FUNCTION();
    // Base Case 1: All tiles from the initial combined pool have been placed into sets
    if (current_pool_tiles.empty()) {
        // Check if all *specific* tiles from tiles_to_add were used
        bool all_added_tiles_used = true;
        if (used_tiles_from_add_pool.size() != original_tiles_to_add_set.size()) {
            all_added_tiles_used = false;
        }
        // Further check: ensure all tiles from original_tiles_to_add_set are indeed in used_tiles_from_add_pool
        // This is implicitly covered if used_tiles_from_add_pool is built correctly.

        return all_added_tiles_used; // If so, a valid arrangement is found
    }

    // Optimization: if current_arrangement already uses more tiles than available, prune
    size_t tiles_in_current_arrangement = 0;
    for(const auto& s : current_arrangement) {
        tiles_in_current_arrangement += s.tiles.size();
    }
    if (tiles_in_current_arrangement > total_tiles_to_place) return false;


    // Iterate through all_possible_valid_sets that can be formed from the *initial* combined pool
    for (const auto& candidate_set : all_possible_valid_sets) {
        // Check if candidate_set can be formed from current_pool_tiles
        std::vector<Tile> temp_pool = current_pool_tiles; // Work with a copy
        bool can_form_set = true;
        std::vector<Tile> tiles_for_this_set;

        for (const auto& tile_needed : candidate_set.tiles) {
            auto it = std::find(temp_pool.begin(), temp_pool.end(), tile_needed);
            if (it != temp_pool.end()) {
                tiles_for_this_set.push_back(*it);
                temp_pool.erase(it); // Remove tile so it can't be used again for this set
            } else {
                can_form_set = false;
                break;
            }
        }

        if (can_form_set) {
            current_arrangement.push_back(candidate_set);

            // Update used_tiles_from_add_pool
            std::set<Tile> newly_used_from_add_pool;
            for(const auto& t : candidate_set.tiles){
                if(original_tiles_to_add_set.count(t)){
                    newly_used_from_add_pool.insert(t);
                    used_tiles_from_add_pool.insert(t);
                }
            }

            // current_pool_tiles was already updated effectively by creating 'temp_pool'
            // and then passing it to the recursive call.
            // The actual current_pool_tiles for this level is 'temp_pool' after forming the set.
            if (find_valid_arrangement_recursive(temp_pool, all_possible_valid_sets, current_arrangement, original_tiles_to_add_set, used_tiles_from_add_pool, total_tiles_to_place)) {
                return true; // Solution found
            }

            // Backtrack
            current_arrangement.pop_back();
            for(const auto& t : newly_used_from_add_pool){ // Remove only those added in this step
                used_tiles_from_add_pool.erase(t);
            }
            // current_pool_tiles is implicitly restored as temp_pool was a copy
        }
    }

    return false; // No solution found from this path
}


// Main function to implement the logic for adding tiles to the board.
// Returns a new BoardState if tiles can be added successfully,
// otherwise returns the original current_board_state.
inline BoardState can_add_tiles_to_board(
    const BoardState& current_board_state,
    const std::vector<Tile>& tiles_to_add
) {
    TRACE_FUNCTION();
    // Step 1: Combine tiles
    std::vector<Tile> current_board_tiles = current_board_state.getAllTiles();
    std::vector<Tile> combined_pool = current_board_tiles;
    combined_pool.insert(combined_pool.end(), tiles_to_add.begin(), tiles_to_add.end());
    std::sort(combined_pool.begin(), combined_pool.end()); // Important for consistency and SetFinder

    // Handle edge case: no tiles to add
    if (tiles_to_add.empty()) {
        // If the current board is valid, return it. Otherwise, it's an interesting case.
        // The problem implies we are trying to make a *new* valid board.
        // If tiles_to_add is empty, the "new" board is just the current board.
        // If current board is already valid, this is fine.
        // If current board is NOT valid, and we add no tiles, we can't make it valid.
        // The prompt says "return the original current_board_state" if a new one isn't found.
        return current_board_state;
    }

    // Ensure tiles_to_add does not contain duplicates not reflected in combined_pool count
    // (e.g. if tiles_to_add = {R1, R1} but only one R1 exists in deck to make it to combined_pool)
    // This is managed by how Tile objects and their equality/comparison are handled.
    // Assuming combined_pool correctly reflects all unique physical tiles.

    std::set<Tile> original_tiles_to_add_set(tiles_to_add.begin(), tiles_to_add.end());
    if (original_tiles_to_add_set.empty() && !tiles_to_add.empty()){
        // This implies tiles_to_add had duplicates that became one in the set, but this check might be redundant
        // if tiles_to_add is always a list of unique tile instances.
    }


    // Step 2: Find all possible valid sets from the combined pool
    std::vector<GameSet> all_possible_valid_sets = SetFinder::find_all_possible_sets(combined_pool);

    // Step 3: Backtracking search
    std::vector<GameSet> result_sets;
    std::set<Tile> used_tiles_from_add_pool; // Track usage of the specific tiles_to_add

    std::vector<Tile> initial_pool_for_recursion = combined_pool; // Make a mutable copy for recursion

    if (find_valid_arrangement_recursive(initial_pool_for_recursion, all_possible_valid_sets, result_sets, original_tiles_to_add_set, used_tiles_from_add_pool, combined_pool.size())) {
        // A valid arrangement forming a new board was found
        // Double check if the found arrangement is actually valid as a whole board
        // The recursion ensures all tiles are used and are in valid sets.
        // is_board_valid will check for uniqueness of tiles across sets in the proposed solution.
        if (is_board_valid(result_sets)) {
             // And ensure all tiles from tiles_to_add were indeed used
            if (used_tiles_from_add_pool.size() == original_tiles_to_add_set.size()) {
                 bool all_physically_added_tiles_present = true;
                 std::multiset<Tile> final_tiles_in_sets;
                 for(const auto& set : result_sets) {
                     for(const auto& tile : set.tiles) {
                         final_tiles_in_sets.insert(tile);
                     }
                 }
                 for(const auto& added_tile : tiles_to_add){ // Check original list, not the set
                     if(final_tiles_in_sets.count(added_tile) == 0){
                         all_physically_added_tiles_present = false;
                         break;
                     }
                     final_tiles_in_sets.erase(final_tiles_in_sets.find(added_tile)); // remove one instance
                 }

                if(all_physically_added_tiles_present) {
                    return BoardState(result_sets);
                }
            }
        }
    }

    // If no solution found, return the original board state
    return current_board_state;
}

} // namespace BoardManipulation
