#pragma once

#include <vector>
#include <algorithm> // For sort, next_permutation
#include <map>       // For grouping by color in findRuns
#include <set>       // To avoid duplicate sets if any
#include "Board.hpp"    // For GameSet, SetType
#include "runs.hpp"     // For existing findRuns (to be adapted/reused)
#include "groups.hpp"   // For existing findGroups (to be completed)
#include "Tile.hpp"     // For Tile

// --- Forward declarations or include completed versions ---
// Assuming findRuns and findGroups will be refactored or new versions created.

namespace SetFinder {

// Helper function to generate all subsets of a specific size (combinations)
// From a vector of tiles. Used for finding groups.
std::vector<std::vector<Tile>> getCombinations(const std::vector<Tile>& tiles, int k) {
    std::vector<std::vector<Tile>> combinations;
    if (k < 0 || k > tiles.size()) {
        return combinations;
    }
    std::vector<bool> v(tiles.size());
    std::fill(v.begin() + v.size() - k, v.end(), true);

    do {
        std::vector<Tile> current_combination;
        for (int i = 0; i < tiles.size(); ++i) {
            if (v[i]) {
                current_combination.push_back(tiles[i]);
            }
        }
        // Sorting here ensures that {T1,T2,T3} is the same as {T3,T1,T2} if GameSet sorts internally
        // However, GameSet constructor already sorts, so this might be redundant if GameSet is used directly.
        // For now, let's keep it to be safe for intermediate std::vector<Tile>
        std::sort(current_combination.begin(), current_combination.end());
        combinations.push_back(current_combination);
    } while (std::next_permutation(v.begin(), v.end()));
    return combinations;
}


// --- Refactored or New findRuns ---
// This function should find all possible valid runs of length 3 or more.
std::vector<GameSet> findAllValidRuns(std::vector<Tile> tiles) {
    std::vector<GameSet> valid_runs;
    if (tiles.size() < 3) {
        return valid_runs;
    }

    // Sort by color, then by number for easier processing of runs
    std::sort(tiles.begin(), tiles.end());

    // Group tiles by color
    std::map<int, std::vector<Tile>> tiles_by_color;
    for (const auto& tile : tiles) {
        tiles_by_color[tile.getColor()].push_back(tile);
    }

    for (const auto& pair_color_tiles : tiles_by_color) {
        // const int color_val = pair_color_tiles.first; // If needed
        const std::vector<Tile>& colored_tiles = pair_color_tiles.second;
        if (colored_tiles.size() < 3) {
            continue;
        }
        // For each color, find all contiguous runs
        // Example: R1 R2 R3 R4 R5 -> R1R2R3, R1R2R3R4, R1R2R3R4R5, R2R3R4, R2R3R4R5, R3R4R5
        for (size_t i = 0; i < colored_tiles.size(); ++i) {
            for (size_t j = i; j < colored_tiles.size(); ++j) {
                // Potential run is from colored_tiles[i] to colored_tiles[j]
                if (j - i + 1 >= 3) { // Minimum length of a run is 3
                    std::vector<Tile> potential_run_segment;
                    bool is_contiguous = true;
                    potential_run_segment.push_back(colored_tiles[i]);
                    for (size_t k = i + 1; k <= j; ++k) {
                        if (colored_tiles[k].getNumber() == colored_tiles[k-1].getNumber() + 1) {
                            potential_run_segment.push_back(colored_tiles[k]);
                        } else {
                            is_contiguous = false;
                            break;
                        }
                    }

                    if (is_contiguous && potential_run_segment.size() >= 3) {
                        GameSet run_set(potential_run_segment, SetType::RUN);
                        // isValidRun will be called by GameSet constructor's isValid implicitly
                        // or we can rely on the GameSet::isValid() method.
                        // The construction logic itself here should ensure it's a valid run.
                        if (run_set.isValid()) { // Double check, though logic should ensure it
                           valid_runs.push_back(run_set);
                        }
                    }
                }
            }
        }
    }
    // Remove duplicate GameSets if any (e.g. if input tiles had duplicates that formed identical runs)
    // Sorting and unique works if GameSet has operator< and operator==
    // Remove duplicate GameSets. GameSet::operator< and GameSet::operator== are now defined.
    std::sort(valid_runs.begin(), valid_runs.end());
    valid_runs.erase(std::unique(valid_runs.begin(), valid_runs.end()), valid_runs.end());
    return valid_runs;
}


// --- Completed findGroups ---
std::vector<GameSet> findAllValidGroups(std::vector<Tile> tiles) {
    std::vector<GameSet> valid_groups;
    if (tiles.size() < 3) {
        return valid_groups;
    }

    std::unordered_map<int, std::vector<Tile>> tiles_by_number;
    for (const auto& tile : tiles) {
        tiles_by_number[tile.getNumber()].push_back(tile);
    }

    for (const auto& pair_number_tiles : tiles_by_number) {
        // const int number = pair_number_tiles.first; // If needed
        const std::vector<Tile>& same_number_tiles = pair_number_tiles.second;
        if (same_number_tiles.size() < 3) {
            continue;
        }

        // Try combinations of 3 tiles
        if (same_number_tiles.size() >= 3) {
            std::vector<std::vector<Tile>> combinations_of_3 = getCombinations(same_number_tiles, 3);
            for (const auto& combo : combinations_of_3) {
                GameSet group_set(combo, SetType::GROUP);
                if (group_set.isValid()) {
                    valid_groups.push_back(group_set);
                }
            }
        }

        // Try combinations of 4 tiles
        if (same_number_tiles.size() >= 4) {
            std::vector<std::vector<Tile>> combinations_of_4 = getCombinations(same_number_tiles, 4);
            for (const auto& combo : combinations_of_4) {
                GameSet group_set(combo, SetType::GROUP);
                if (group_set.isValid()) {
                    valid_groups.push_back(group_set);
                }
            }
        }
    }
     // To ensure uniqueness if GameSet has operator< and operator==
    // std::sort(valid_groups.begin(), valid_groups.end());
    // valid_groups.erase(std::unique(valid_groups.begin(), valid_groups.end()), valid_groups.end());
    // A std::set<GameSet> is better for uniqueness. For now, we might get duplicates if the input
    // tiles can form the same group in multiple ways (e.g. if input has R1,B1,Y1,P1,R1_dup,B1_dup).
    // GameSet sorts its internal tiles, so {R1,B1,Y1} is same as {B1,Y1,R1}.
    // If `valid_groups` is a std::vector<GameSet>, we need to sort and unique.
    // If it's a std::set<GameSet>, it handles uniqueness if GameSet::operator< is defined.
    // Let's use std::set temporarily to handle uniqueness, then convert to vector.
    // Requires GameSet::operator<
    // Remove duplicate GameSets. GameSet::operator< and GameSet::operator== are now defined.
    std::sort(valid_groups.begin(), valid_groups.end());
    valid_groups.erase(std::unique(valid_groups.begin(), valid_groups.end()), valid_groups.end());
    return valid_groups;
}


// --- Main function for this step ---
// Changed sub-function params to take const& and copy internally if modification is needed.
// However, findRuns/findGroups already take by value, which makes a copy.
// So, find_all_possible_sets can just pass its const& argument.
std::vector<GameSet> find_all_possible_sets(const std::vector<Tile>& input_tiles) {
    std::vector<GameSet> all_sets;

    // Pass input_tiles (const ref). The by-value parameters in the helper functions
    // will create copies for them to work with.
    std::vector<GameSet> runs = findAllValidRuns(input_tiles);
    all_sets.insert(all_sets.end(), runs.begin(), runs.end());

    std::vector<GameSet> groups = findAllValidGroups(input_tiles);
    all_sets.insert(all_sets.end(), groups.begin(), groups.end());

    // Consolidate all found sets and remove duplicates
    std::sort(all_sets.begin(), all_sets.end());
    all_sets.erase(std::unique(all_sets.begin(), all_sets.end()), all_sets.end());

    return all_sets;
}

} // namespace SetFinder
