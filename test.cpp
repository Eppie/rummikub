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

void testIsBoardValidFunction() {
    std::cout << "\n--- Testing is_board_valid (free function) ---" << std::endl;

    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red);
    Tile b3(3,blue), y3(3,yellow); // p3 is purple, defined in Tile.hpp's enum
    Tile p3_tile(3, purple); // Using the enum name directly

    GameSet valid_run({r1,r2,r3}, SetType::RUN);         // R1 R2 R3
    GameSet valid_group({r4,Tile(4,blue),Tile(4,yellow)}, SetType::GROUP); // R4 B4 Y4
    GameSet invalid_set_short_run({r1,r2}, SetType::RUN); // Invalid run (too short)
    GameSet group_with_r1({r1, Tile(1,blue), Tile(1,yellow)}, SetType::GROUP); // R1 B1 Y1 - valid on its own

    // Test Case 1: Empty vector of sets
    std::vector<GameSet> empty_sets;
    assert(is_board_valid(empty_sets));
    std::cout << "TC1 Empty vector of sets: Passed" << std::endl;

    // Test Case 2: All valid sets and unique tiles
    std::vector<GameSet> valid_board_sets = {valid_run, valid_group};
    assert(is_board_valid(valid_board_sets));
    std::cout << "TC2 All valid sets, unique tiles: Passed" << std::endl;

    // Test Case 3: Contains an invalid set
    std::vector<GameSet> board_with_invalid_set = {valid_run, invalid_set_short_run};
    assert(!is_board_valid(board_with_invalid_set));
    std::cout << "TC3 Contains an invalid set: Passed" << std::endl;

    // Test Case 4: All valid sets but with duplicate tiles
    // valid_run contains R1, group_with_r1 also contains R1.
    std::vector<GameSet> board_with_duplicate_tiles = {valid_run, group_with_r1};
    assert(valid_run.isValid()); // Sanity check
    assert(group_with_r1.isValid()); // Sanity check
    assert(!is_board_valid(board_with_duplicate_tiles));
    std::cout << "TC4 Valid sets, but duplicate tiles: Passed" << std::endl;

    // Test Case 5: Single valid set
    std::vector<GameSet> single_valid_set = {valid_run};
    assert(is_board_valid(single_valid_set));
    std::cout << "TC5 Single valid set: Passed" << std::endl;

    // Test Case 6: Single invalid set
    std::vector<GameSet> single_invalid_set = {invalid_set_short_run};
    assert(!is_board_valid(single_invalid_set));
    std::cout << "TC6 Single invalid set: Passed" << std::endl;


    std::cout << "--- is_board_valid (free function) Tests Passed ---" << std::endl;
}

// Helper to compare BoardState objects for testing can_add_tiles_to_board
// This is tricky because the order of sets might not be guaranteed.
// A robust check would sort the sets within each BoardState before comparison,
// or count occurrences of each unique set.
// For now, we'll sort the sets vector before comparison.
bool areBoardStatesEquivalent(BoardState bs1, BoardState bs2) {
    // First, check if both boards are valid according to their own rules.
    // (This is more of a sanity check on the test inputs/outputs if they are expected to be valid)
    // assert(bs1.isValidBoard());
    // assert(bs2.isValidBoard());

    if (bs1.sets.size() != bs2.sets.size()) {
        return false;
    }
    // Sort sets for comparison
    std::sort(bs1.sets.begin(), bs1.sets.end());
    std::sort(bs2.sets.begin(), bs2.sets.end());
    return bs1.sets == bs2.sets;
}


void testCanAddTilesToBoard() {
    std::cout << "\n--- Testing can_add_tiles_to_board ---" << std::endl;

    // Define some tiles for testing
    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red), r5(5,red), r6(6,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue), b4(4,blue);
    Tile y1(1,yellow), y2(2,yellow), y3(3,yellow), y4(4,yellow), y5(5,yellow); // Added y5 definition
    Tile p1(1,purple), p2(2,purple), p3(3,purple), p4(4,purple);

    // Test Case 1: Empty board, add tiles that form a valid set
    BoardState empty_board;
    std::vector<Tile> tiles_to_add_1 = {r1, r2, r3};
    BoardState expected_board_1_sets;
    expected_board_1_sets.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    BoardState result_board_1 = BoardManipulation::can_add_tiles_to_board(empty_board, tiles_to_add_1);
    assert(areBoardStatesEquivalent(result_board_1, expected_board_1_sets));
    std::cout << "TC1 Empty board, add valid set: Passed" << std::endl;

    // Test Case 2: Empty board, add tiles that cannot form any valid set
    std::vector<Tile> tiles_to_add_2 = {r1, r2}; // Not enough for a run
    BoardState result_board_2 = BoardManipulation::can_add_tiles_to_board(empty_board, tiles_to_add_2);
    assert(areBoardStatesEquivalent(result_board_2, empty_board)); // Should return original empty board
    std::cout << "TC2 Empty board, add invalid set: Passed" << std::endl;

    // Test Case 3: Existing board, add tiles that form a new, separate valid set
    BoardState board_3;
    board_3.addSet(GameSet({b1,b2,b3}, SetType::RUN)); // B1,B2,B3
    std::vector<Tile> tiles_to_add_3 = {r1, r2, r3};   // R1,R2,R3
    BoardState expected_board_3_sets;
    expected_board_3_sets.addSet(GameSet({b1,b2,b3}, SetType::RUN));
    expected_board_3_sets.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    BoardState result_board_3 = BoardManipulation::can_add_tiles_to_board(board_3, tiles_to_add_3);
    assert(areBoardStatesEquivalent(result_board_3, expected_board_3_sets));
    std::cout << "TC3 Existing board, add separate valid set: Passed" << std::endl;

    // Test Case 4: Existing board, add tiles that cannot be legally placed
    BoardState board_4; // Board has B1,B2,B3
    board_4.addSet(GameSet({b1,b2,b3}, SetType::RUN));
    std::vector<Tile> tiles_to_add_4 = {r1, r2}; // Try to add R1,R2 (invalid set)
    BoardState result_board_4 = BoardManipulation::can_add_tiles_to_board(board_4, tiles_to_add_4);
    assert(areBoardStatesEquivalent(result_board_4, board_4)); // Should return original board
    std::cout << "TC4 Existing board, add tiles that cannot be placed: Passed" << std::endl;

    // Test Case 5: Add tiles that complete an existing set (conceptually, though current logic rebuilds)
    // Board has R1, R2. Add R3. Expected: R1,R2,R3
    BoardState board_5;
    board_5.addSet(GameSet({r1,r2,r3}, SetType::RUN)); // Start with R1,R2,R3 for simplicity, then try to "add" R4
                                                      // The function will combine all and try to rebuild.
                                                      // Let's make a board that can be extended.
    BoardState current_board_5; // Empty for now.
    // We want to test if {R1,R2} + {R3} -> {R1,R2,R3}
    // But the setup requires the current board to be valid.
    // Let's test breaking and reforming.
    // Board: {R1,R2,R3}, {B1,B2,B3}. Add: R4.
    // Expected: {R1,R2,R3,R4}, {B1,B2,B3}
    BoardState board_for_5;
    board_for_5.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    board_for_5.addSet(GameSet({b1,b2,b3}, SetType::RUN));
    std::vector<Tile> tiles_to_add_5 = {r4}; // Add R4
    BoardState expected_board_5_sets;
    expected_board_5_sets.addSet(GameSet({r1,r2,r3,r4}, SetType::RUN));
    expected_board_5_sets.addSet(GameSet({b1,b2,b3}, SetType::RUN));
    BoardState result_board_5 = BoardManipulation::can_add_tiles_to_board(board_for_5, tiles_to_add_5);
    result_board_5.print();
    expected_board_5_sets.print();
    assert(areBoardStatesEquivalent(result_board_5, expected_board_5_sets));
    std::cout << "TC5 Add tile that extends an existing run (via rebuild): Passed" << std::endl;


    // Test Case 6: Add tiles requiring breaking and reforming multiple sets
    // Board: {R1,R2,R3}, {R4,B4,Y4}. Add: R5,R6.
    // Expected: {R1,R2,R3,R4,R5,R6}, {B4,Y4} (no, B4,Y4 is not valid)
    // Expected: {R1,R2,R3,R4,R5,R6} and B4, Y4 are left over, so this should fail if B4,Y4 cannot form new set
    // Let's try: Board: {R1,R2,R3}, {B4,Y4,P4}. Add: R4.
    // Expected: {R1,R2,R3,R4}, {B4,Y4,P4}. (Similar to TC5, but R4 is taken from a group)
    BoardState board_for_6;
    board_for_6.addSet(GameSet({b1,b2,b3}, SetType::RUN)); // B1,B2,B3
    board_for_6.addSet(GameSet({r4,y4,p4}, SetType::GROUP)); // R4,Y4,P4
    std::vector<Tile> tiles_to_add_6 = {r1,r2,r3}; // Add R1,R2,R3. R4 is on board.
                                                // Combined: B1,B2,B3, R4,Y4,P4, R1,R2,R3
                                                // Expected: {B1,B2,B3}, {Y4,P4,r4}, {R1,R2,R3} - no, this is not right.
                                                // Expected: {B1,B2,B3}, {R1,R2,R3,R4}, {Y4,P4} (Y4,P4 is not valid) -> fail
                                                // The goal is to use R1,R2,R3.
                                                // Pool: B1,B2,B3, R4,Y4,P4, R1,R2,R3.
                                                // Possible new arrangement: {B1,B2,B3}, {R1,R2,R3}, {R4,Y4,P4}. This is a valid new board.
                                                // This is like TC3.

    // Let's try a real break/reform:
    // Board: {R1,R2,R3,R4}, {Y5,Y6,Y7}. Add: B4, P4.
    // Pool: R1,R2,R3,R4, Y5,Y6,Y7, B4, P4.
    // Added tiles B4, P4 must be used.
    // New sets: {R1,R2,R3}, {R4,B4,P4}, {Y5,Y6,Y7}. This is a valid recombination.
    BoardState board_6_current;
    board_6_current.addSet(GameSet({r1,r2,r3,r4}, SetType::RUN)); // R1,R2,R3,R4
    board_6_current.addSet(GameSet({y5,Tile(6,yellow),Tile(7,yellow)}, SetType::RUN)); // Y5,Y6,Y7
    std::vector<Tile> tiles_to_add_6_real = {b4, p4}; // Add B4, P4

    BoardState expected_board_6_sets;
    expected_board_6_sets.addSet(GameSet({r1,r2,r3}, SetType::RUN)); // R1,R2,R3
    expected_board_6_sets.addSet(GameSet({r4,b4,p4}, SetType::GROUP)); // R4,B4,P4
    expected_board_6_sets.addSet(GameSet({y5,Tile(6,yellow),Tile(7,yellow)}, SetType::RUN)); // Y5,Y6,Y7

    BoardState result_board_6 = BoardManipulation::can_add_tiles_to_board(board_6_current, tiles_to_add_6_real);
    result_board_6.print();
    expected_board_6_sets.print();
    assert(areBoardStatesEquivalent(result_board_6, expected_board_6_sets));
    std::cout << "TC6 Add tiles requiring break and reform: Passed" << std::endl;

    // Test Case 7: Tiles cannot be legally added (results in original board)
    // Board: {R1,R2,R3}. Add: B1, B2. (B1,B2 cannot form a set, existing set is untouched)
    BoardState board_7_current;
    board_7_current.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    std::vector<Tile> tiles_to_add_7 = {b1,b2};
    BoardState result_board_7 = BoardManipulation::can_add_tiles_to_board(board_7_current, tiles_to_add_7);
    assert(areBoardStatesEquivalent(result_board_7, board_7_current));
    std::cout << "TC7 Add tiles that cannot be legally placed (partial set): Passed" << std::endl;

    // Test Case 8: Empty tiles_to_add (should return original board)
    BoardState board_8_current;
    board_8_current.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    std::vector<Tile> tiles_to_add_8 = {}; // Empty
    BoardState result_board_8 = BoardManipulation::can_add_tiles_to_board(board_8_current, tiles_to_add_8);
    assert(areBoardStatesEquivalent(result_board_8, board_8_current));
    std::cout << "TC8 Empty tiles_to_add: Passed" << std::endl;

    // Test Case 9: Adding tiles that *must* be used but cannot be part of a full valid board
    // Board: {R1,R2,R3}. Add: B4. (B4 cannot form any set with R1,R2,R3 and itself)
    BoardState board_9_current;
    board_9_current.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    std::vector<Tile> tiles_to_add_9 = {b4};
    BoardState result_board_9 = BoardManipulation::can_add_tiles_to_board(board_9_current, tiles_to_add_9);
    assert(areBoardStatesEquivalent(result_board_9, board_9_current));
    std::cout << "TC9 Added tile must be used but cannot complete a board: Passed" << std::endl;


    std::cout << "--- can_add_tiles_to_board Tests Passed (Initial Set) ---" << std::endl;
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
    std::cout << "\nAttempting to run is_board_valid (free function) tests..." << std::endl;
    testIsBoardValidFunction();
    std::cout << "\nAttempting to run can_add_tiles_to_board tests..." << std::endl;
    testCanAddTilesToBoard();

	return 0;
}

