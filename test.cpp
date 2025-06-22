/*
 * =====================================================================================
 *
 *       Filename:  test.cpp
 *
 *    Description:  Test cases for basic functionality
 *
 *        Version:  1.0
 *        Created:  12/23/2015 01:32:00 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Andrew Epstein
 *
 * =====================================================================================
 */

#include "utilities.hpp"
#include "Board.hpp" // Include the new Board structures
#include <cassert>
#include <iostream> // For printing in tests

// Existing global variable
vector<Tile> allTiles = generateAllTiles();

// Forward declare testSetFinder if main is defined before it.
// void testSetFinder();

void testBoardStructures() {
    std::cout << "\n--- Testing Board Structures ---" << std::endl;

    // Create some sample tiles
    Tile r1(1, red);
    Tile r2(2, red);
    Tile r3(3, red);
    Tile r4(4, red);

    Tile b3(3, blue);
    Tile y3(3, yellow);
    // Tile p3(3, purple); // Defined in Tile.hpp, but color enum is local to Tile.hpp
                         // For robust testing, might need to reference colors more directly
                         // or ensure color enum is more globally accessible.
                         // Using int values for now based on Tile.hpp enum.
    Tile p3_alt(3, 2); // Assuming purple = 2 from Tile.hpp enum color

    // Test GameSet - Run
    std::vector<Tile> run_tiles = {r1, r2, r3};
    GameSet run_set(run_tiles, SetType::RUN);
    std::cout << "Testing Run: ";
    run_set.print();
    assert(run_set.isValid());
    assert(run_set.type == SetType::RUN);
    assert(run_set.tiles.size() == 3);

    // Test GameSet - Group
    std::vector<Tile> group_tiles = {r3, b3, y3}; // r3 is (3, red), b3 is (3, blue), y3 is (3, yellow)
    GameSet group_set(group_tiles, SetType::GROUP);
    std::cout << "Testing Group: ";
    group_set.print();
    assert(group_set.isValid());
    assert(group_set.type == SetType::GROUP);

    // Test GameSet - Invalid Run (gap)
    std::vector<Tile> invalid_run_tiles = {r1, r3, r4};
    GameSet invalid_run_set(invalid_run_tiles, SetType::RUN);
    std::cout << "Testing Invalid Run (gap): ";
    invalid_run_set.print();
    assert(!invalid_run_set.isValid());

    // Test GameSet - Invalid Group (duplicate color)
    std::vector<Tile> invalid_group_tiles = {r3, b3, r1}; // r1 is (1,red), r3 is (3,red) - numbers different
                                                          // For same number, different color: r3, b3, Tile(3,red)
    Tile r3_dup(3,red);
    std::vector<Tile> invalid_group_tiles_dup_color = {r3, b3, r3_dup};
    GameSet invalid_group_set_dup_color(invalid_group_tiles_dup_color, SetType::GROUP);
    std::cout << "Testing Invalid Group (duplicate color): ";
    invalid_group_set_dup_color.print();
    assert(!invalid_group_set_dup_color.isValid());


    // Test BoardState
    BoardState board;
    std::cout << "\nInitial Board State:" << std::endl;
    board.print();

    board.addSet(run_set);
    board.addSet(group_set);
    std::cout << "\nBoard State after adding sets:" << std::endl;
    board.print();

    std::vector<Tile> all_board_tiles = board.getAllTiles();
    std::cout << "All tiles on board (" << all_board_tiles.size() << "): ";
    for(const auto& t : all_board_tiles) {
        t.print();
        std::cout << " ";
    }
    std::cout << std::endl;
    assert(all_board_tiles.size() == run_set.tiles.size() + group_set.tiles.size());

    // Test equality operators
    GameSet run_set_copy(run_tiles, SetType::RUN);
    assert(run_set == run_set_copy);

    BoardState board_copy;
    board_copy.addSet(run_set_copy);
    board_copy.addSet(group_set); // Assuming group_set was also valid and added
    // Note: BoardState::operator== depends on order. If addSet reorders or if sets are added differently, this might fail.
    // For this simple test, it should pass.
    assert(board.sets.size() == 2); // Ensure sets were added
    assert(board_copy.sets.size() == 2);
    if (board.sets.size() == board_copy.sets.size()) { // only check equality if sizes match
      assert(board == board_copy);
    }


    std::cout << "--- Board Structures Tests Passed ---" << std::endl;
}

// --- Tests for SetFinder ---
#include "SetFinder.hpp" // Include the SetFinder functions

// Helper to compare vectors of GameSet for testing purposes
// Sorts both vectors then compares them.
bool areGameSetVectorsEqual(const std::string& tc_name, std::vector<GameSet> v1, std::vector<GameSet> v2) {
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    if (v1.size() != v2.size()) {
        std::cout << "Debug [" << tc_name << "]: Size mismatch. Actual size: " << v1.size() << ", Expected size: " << v2.size() << std::endl;
        std::cout << "Actual sets:" << std::endl;
        for(const auto& s : v1) { s.print(); }
        std::cout << "Expected sets:" << std::endl;
        for(const auto& s : v2) { s.print(); }
        return false;
    }
    if (v1 != v2) { // Relies on GameSet::operator==
        std::cout << "Debug [" << tc_name << "]: Content mismatch after sorting." << std::endl;
        std::cout << "Actual sets:" << std::endl;
        for(const auto& s : v1) { s.print(); }
        std::cout << "Expected sets:" << std::endl;
        for(const auto& s : v2) { s.print(); }
        return false;
    }
    return true;
}

void testSetFinder() {
    std::cout << "\n--- Testing SetFinder ---" << std::endl;
    std::cout << "Starting testSetFinder..." << std::endl;

    // Tiles (using enum values from Tile.hpp for colors)
    // enum color { blue = 1, purple = 2, red = 3, yellow = 4 };
    Tile r1(1, red), r2(2, red), r3(3, red), r4(4, red), r5(5, red);
    Tile b1(1, blue), b2(2, blue), b3(3, blue), b4(4, blue);
    Tile y3(3, yellow), y5(5, yellow);
    Tile p3(3, purple), p7(7, purple); // purple is 2

    // Test Case 1: Basic Run
    std::vector<Tile> tc1_tiles = {r1, r2, r3};
    std::vector<GameSet> tc1_expected = {GameSet({r1, r2, r3}, SetType::RUN)};
    std::vector<GameSet> tc1_actual = SetFinder::find_all_possible_sets(tc1_tiles);
    assert(areGameSetVectorsEqual("TC1 Basic Run", tc1_actual, tc1_expected));
    std::cout << "TC1 Basic Run: Passed" << std::endl;

    // Test Case 2: Multiple Runs from a sequence
    std::vector<Tile> tc2_tiles = {b1, b2, b3, b4};
    std::vector<GameSet> tc2_expected = {
        GameSet({b1, b2, b3}, SetType::RUN),
        GameSet({b1, b2, b3, b4}, SetType::RUN),
        GameSet({b2, b3, b4}, SetType::RUN)
    };
    std::vector<GameSet> tc2_actual = SetFinder::find_all_possible_sets(tc2_tiles);
    assert(areGameSetVectorsEqual("TC2 Multiple Runs", tc2_actual, tc2_expected));
    std::cout << "TC2 Multiple Runs: Passed" << std::endl;

    // Test Case 3: Basic Group
    std::vector<Tile> tc3_tiles = {r3, b3, y3};
    std::vector<GameSet> tc3_expected = {GameSet({b3, r3, y3}, SetType::GROUP)}; // Order in GameSet is sorted
    std::vector<GameSet> tc3_actual = SetFinder::find_all_possible_sets(tc3_tiles);
    assert(areGameSetVectorsEqual("TC3 Basic Group", tc3_actual, tc3_expected));
    std::cout << "TC3 Basic Group: Passed" << std::endl;

    // Test Case 4: Multiple Groups from same numbers
    Tile r7(7, red), b7(7, blue), y7(7, yellow); // p7 already defined
    std::vector<Tile> tc4_tiles = {r7, b7, y7, p7};
    std::vector<GameSet> tc4_expected = {
        GameSet({b7, p7, r7}, SetType::GROUP),    // B P R
        GameSet({b7, p7, y7}, SetType::GROUP),    // B P Y
        GameSet({b7, r7, y7}, SetType::GROUP),    // B R Y
        GameSet({p7, r7, y7}, SetType::GROUP),    // P R Y
        GameSet({b7, p7, r7, y7}, SetType::GROUP) // B P R Y
    };
    std::vector<GameSet> tc4_actual = SetFinder::find_all_possible_sets(tc4_tiles);
    assert(areGameSetVectorsEqual("TC4 Multiple Groups", tc4_actual, tc4_expected));
    std::cout << "TC4 Multiple Groups: Passed" << std::endl;

    // Test Case 5: Mixed Tiles (Run and Group)
    std::vector<Tile> tc5_tiles = {r1, r2, r3, b3, y3};
    std::vector<GameSet> tc5_expected = {
        GameSet({r1, r2, r3}, SetType::RUN),
        GameSet({b3, r3, y3}, SetType::GROUP) // Tiles inside GameSet are sorted: b3, r3, y3
    };
    std::vector<GameSet> tc5_actual = SetFinder::find_all_possible_sets(tc5_tiles);
    assert(areGameSetVectorsEqual("TC5 Mixed Run/Group", tc5_actual, tc5_expected));
    std::cout << "TC5 Mixed Run/Group: Passed" << std::endl;

    // Test Case 6: Tiles with Duplicates
    Tile r1_dup(1, red); // Same as r1
    std::vector<Tile> tc6_tiles = {r1, r1_dup, r2, r3, b1, y3}; // y3 instead of y1 to avoid another group for now
    std::vector<GameSet> tc6_expected = {
        GameSet({r1, r2, r3}, SetType::RUN) // Should be unique
    };
    std::vector<GameSet> tc6_actual = SetFinder::find_all_possible_sets(tc6_tiles);
    assert(areGameSetVectorsEqual("TC6 Duplicates (Run)", tc6_actual, tc6_expected));
    std::cout << "TC6 Duplicates (Run): Passed" << std::endl;

    // Test Case 6b: Duplicates forming a group
    Tile b1_dup(1, blue);
    Tile y1_tile(1, yellow); // Explicitly create y1
    std::vector<Tile> tc6b_tiles = {r1, r1_dup, b1, b1_dup, y1_tile};
    std::vector<GameSet> tc6b_expected = {
        GameSet({b1, r1, y1_tile}, SetType::GROUP) // Sorted: B R Y
    };
    std::vector<GameSet> tc6b_actual = SetFinder::find_all_possible_sets(tc6b_tiles);
    assert(areGameSetVectorsEqual("TC6b Duplicates (Group)", tc6b_actual, tc6b_expected));
    std::cout << "TC6b Duplicates (Group): Passed" << std::endl;


    // Test Case 7: No Sets
    std::vector<Tile> tc7_tiles = {r1, b2, y5}; // y5 to avoid accidental run/group
    std::vector<GameSet> tc7_expected = {};
    std::vector<GameSet> tc7_actual = SetFinder::find_all_possible_sets(tc7_tiles);
    assert(areGameSetVectorsEqual("TC7 No Sets", tc7_actual, tc7_expected));
    std::cout << "TC7 No Sets: Passed" << std::endl;

    // Test Case 8: Only two tiles
    std::vector<Tile> tc8_tiles = {r1, r2};
    std::vector<GameSet> tc8_expected = {};
    std::vector<GameSet> tc8_actual = SetFinder::find_all_possible_sets(tc8_tiles);
    assert(areGameSetVectorsEqual("TC8 Two Tiles", tc8_actual, tc8_expected));
    std::cout << "TC8 Two Tiles: Passed" << std::endl;

    std::cout << "--- SetFinder Tests Passed ---" << std::endl;
}

void testIsValidBoard() {
    std::cout << "\n--- Testing BoardState::isValidBoard ---" << std::endl;

    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red);
    Tile b3(3,blue), y3(3,yellow), p3(3,purple);

    GameSet valid_run({r1,r2,r3}, SetType::RUN);         // R1 R2 R3
    GameSet valid_group({r4,Tile(4,blue),Tile(4,yellow)}, SetType::GROUP); // R4 B4 Y4
    GameSet invalid_run({r1,r3,r4}, SetType::RUN);     // R1 R3 R4 (invalid)
    GameSet group_with_r1({r1, Tile(1,blue), Tile(1,yellow)}, SetType::GROUP); // R1 B1 Y1

    // Test Case 1: Empty Board
    BoardState board1;
    assert(board1.isValidBoard());
    std::cout << "TC1 Empty Board: Passed" << std::endl;

    // Test Case 2: Valid Board (multiple valid, distinct sets)
    // BoardState's addSet only adds valid sets.
    BoardState board2;
    board2.addSet(valid_run);   // R1 R2 R3
    board2.addSet(valid_group); // R4 B4 Y4
    assert(board2.isValidBoard());
    std::cout << "TC2 Valid Board: Passed" << std::endl;

    // Test Case 3: Invalid Board (one set is invalid)
    // To test this, we need to construct BoardState with an invalid set directly,
    // as addSet would filter it out.
    std::vector<GameSet> sets_for_board3 = {valid_run, invalid_run};
    BoardState board3(sets_for_board3);
    assert(!board3.isValidBoard());
    std::cout << "TC3 Invalid Set on Board: Passed" << std::endl;

    // Test Case 4: Invalid Board (tile duplication across sets)
    // R1 is in valid_run (R1 R2 R3) and group_with_r1 (R1 B1 Y1)
    std::vector<GameSet> sets_for_board4 = {valid_run, group_with_r1};
    BoardState board4(sets_for_board4);
    // Both sets are individually valid, but share R1.
    assert(valid_run.isValid());
    assert(group_with_r1.isValid());
    assert(!board4.isValidBoard()); // Should be invalid due to R1 duplication
    std::cout << "TC4 Tile Duplication Across Sets: Passed" << std::endl;

    // Test Case 5: Valid board with more sets
    GameSet valid_run_2({Tile(5,blue), Tile(6,blue), Tile(7,blue)}, SetType::RUN);
    BoardState board5;
    board5.addSet(valid_run);
    board5.addSet(valid_group);
    board5.addSet(valid_run_2);
    assert(board5.isValidBoard());
    std::cout << "TC5 Valid Board Multiple Sets: Passed" << std::endl;


    std::cout << "--- BoardState::isValidBoard Tests Passed ---" << std::endl;
}


// Original main function modified to include all tests
int main() {
	assert( allTiles.size() == 104 );
	std::cout << "Initial tile generation test passed." << std::endl;

	testBoardStructures();
    std::cout << "\nAttempting to run SetFinder tests..." << std::endl; // Debug print
	testSetFinder();
    std::cout << "\nAttempting to run isValidBoard tests..." << std::endl; // Debug print
    testIsValidBoard();

	return 0;
}

