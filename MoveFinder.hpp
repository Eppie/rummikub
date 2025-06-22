#pragma once

#include <vector>
#include <algorithm> // For std::sort, std::next_permutation, std::remove_if
#include <optional>  // For std::optional
#include <set>       // For std::set to handle tile uniqueness when calculating remaining hand

#include "GameTypes.hpp"         // For Move, Tile, BoardState
#include "Board.hpp"             // For BoardManipulation::can_add_tiles_to_board and BoardState
#include "PerformanceTracer.hpp" // For TRACE_FUNCTION

namespace MoveFinder {

// Helper function to generate all non-empty subsets of a given set of tiles.
// Subsets are returned sorted by size in descending order.
std::vector<std::vector<Tile>> generate_tile_subsets_descending(const std::vector<Tile>& tiles) {
    TRACE_FUNCTION();
    std::vector<std::vector<Tile>> all_subsets;
    if (tiles.empty()) {
        return all_subsets;
    }

    int n = tiles.size();
    // Iterate from 1 to 2^n - 1 (representing all non-empty subsets)
    for (int i = 1; i < (1 << n); ++i) {
        std::vector<Tile> current_subset;
        for (int j = 0; j < n; ++j) {
            // Check if the j-th bit is set in i
            if ((i >> j) & 1) {
                current_subset.push_back(tiles[j]);
            }
        }
        // Ensure tiles within a subset are sorted for consistency, although
        // can_add_tiles_to_board's use of combined_pool sorting might make this redundant.
        // std::sort(current_subset.begin(), current_subset.end()); // GameSet constructor sorts.
        all_subsets.push_back(current_subset);
    }

    // Sort subsets by size in descending order
    std::sort(all_subsets.begin(), all_subsets.end(), [](const std::vector<Tile>& a, const std::vector<Tile>& b) {
        return a.size() > b.size();
    });

    return all_subsets;
}

// Helper function to calculate the remaining hand after playing a subset of tiles
std::vector<Tile> calculate_remaining_hand(const std::vector<Tile>& original_hand, const std::vector<Tile>& played_tiles) {
    TRACE_FUNCTION();
    std::vector<Tile> remaining_hand = original_hand;

    // For each tile in played_tiles, remove one instance from remaining_hand.
    // Using a multiset approach for correctness if hands can have duplicate distinguishable tiles (though Rummikub usually not)
    // or if played_tiles might not be perfectly unique from original_hand due to how subsets are formed.
    // A simpler approach if all tiles are unique instances (which they are in Rummikub):
    for (const auto& played_tile : played_tiles) {
        auto it = std::find(remaining_hand.begin(), remaining_hand.end(), played_tile);
        if (it != remaining_hand.end()) {
            remaining_hand.erase(it); // Erases the first found instance
        }
    }
    std::sort(remaining_hand.begin(), remaining_hand.end()); // Keep hand sorted for consistency
    return remaining_hand;
}


// Finds the best move for a player given the current board state and their hand.
// The "best" move is defined as the one that plays the most tiles from the player's hand.
// Returns std::nullopt if no move is possible.
std::optional<Move> find_best_move(
    const BoardState& current_board_state,
    const std::vector<Tile>& current_hand) {
    TRACE_FUNCTION();

    if (current_hand.empty()) {
        return std::nullopt; // Cannot make a move with an empty hand.
    }

    std::vector<Tile> sorted_hand = current_hand; // Work with a sorted copy
    std::sort(sorted_hand.begin(), sorted_hand.end());

    std::vector<std::vector<Tile>> hand_subsets = generate_tile_subsets_descending(sorted_hand);

    // std::vector<Tile> initial_board_tiles = current_board_state.getAllTiles(); // No longer needed for comparison here
    // std::cout << "find_best_move: Starting with hand size " << sorted_hand.size() << std::endl;

    for (const auto& tiles_to_try_playing : hand_subsets) {
        if (tiles_to_try_playing.empty()) {
            continue;
        }

        // std::cout << "find_best_move: Trying to play subset of size " << tiles_to_try_playing.size() << ": ";
        // for(const auto& t : tiles_to_try_playing) { t.print(); std::cout << " "; }
        // std::cout << std::endl;

        // Attempt to add these tiles to the board
        std::optional<BoardState> potential_new_board_state_opt = BoardManipulation::can_add_tiles_to_board(current_board_state, tiles_to_try_playing);

        // if (potential_new_board_state_opt) {
        //     std::cout << "find_best_move: SUCCESS with subset size " << tiles_to_try_playing.size() << std::endl;
        //     // Successfully played this subset of tiles
        //     // The can_add_tiles_to_board function now guarantees that if it returns a BoardState,
        //     // it's valid and incorporates the tiles_to_try_playing.
        //     std::vector<Tile> remaining_hand = calculate_remaining_hand(sorted_hand, tiles_to_try_playing);
        //     return Move(*potential_new_board_state_opt, remaining_hand, static_cast<int>(tiles_to_try_playing.size()));
        // } else {
        //     std::cout << "find_best_move: FAILED with subset size " << tiles_to_try_playing.size() << std::endl;
        // }
        if (potential_new_board_state_opt) {
            std::vector<Tile> remaining_hand = calculate_remaining_hand(sorted_hand, tiles_to_try_playing);
            return Move(*potential_new_board_state_opt, remaining_hand, static_cast<int>(tiles_to_try_playing.size()));
        }
    }
    // std::cout << "find_best_move: No move found after trying all subsets." << std::endl;
    return std::nullopt; // No valid move found
}

} // namespace MoveFinder
