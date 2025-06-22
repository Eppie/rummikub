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
#include "Board.hpp"
#include "PerformanceTracer.hpp"
#include "SetFinder.hpp"      // Original include
#include "MoveFinder.hpp"     // Added for new tests
#include "GameTypes.hpp"      // Added for Move struct, Tile, etc. (Board.hpp also includes it)

#include <cassert>
#include <iostream>
#include <algorithm> // For std::sort
#include <vector>    // For std::vector
#include <set>       // For std::set in test comparisons (already used by Board.hpp)
#include <optional>  // For std::optional (already used by Board.hpp)


// Existing global variable
vector<Tile> allTiles = generateAllTiles();

// Helper to sort a vector of tiles (useful for comparing hands)
std::vector<Tile> sorted(std::vector<Tile> tiles) {
    std::sort(tiles.begin(), tiles.end());
    return tiles;
}

// Helper to compare vectors of GameSet for testing purposes
// Sorts both vectors then compares them.
// Added verbose parameter from later iterations.
bool areGameSetVectorsEqual(const std::string& tc_name, std::vector<GameSet> v1, std::vector<GameSet> v2, bool verbose = false) {
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    if (v1.size() != v2.size() || v1 != v2) { // Relies on GameSet::operator==
        if (verbose) {
            std::cout << "Debug [" << tc_name << "]: GameSet vectors not equal." << std::endl;
            std::cout << "Actual (size " << v1.size() << "):" << std::endl; for(const auto& s : v1) s.print();
            std::cout << "Expected (size " << v2.size() << "):" << std::endl; for(const auto& s : v2) s.print();
        }
        return false;
    }
    return true;
}

// Helper to compare BoardState objects for testing can_add_tiles_to_board
// Updated to use the verbose areGameSetVectorsEqual
bool areBoardStatesEquivalent(BoardState bs1, BoardState bs2, bool verbose = false) {
    // Sorting sets for comparison as order might not be guaranteed.
    std::sort(bs1.sets.begin(), bs1.sets.end());
    std::sort(bs2.sets.begin(), bs2.sets.end());
    if (bs1.sets.size() != bs2.sets.size()){
        if(verbose){
            std::cout << "Debug [BoardStateComparison]: Size mismatch. Actual size: " << bs1.sets.size() << ", Expected size: " << bs2.sets.size() << std::endl;
            std::cout << "Actual board sets:" << std::endl; bs1.print();
            std::cout << "Expected board sets:" << std::endl; bs2.print();
        }
        return false;
    }
    if (bs1.sets != bs2.sets) { // Relies on GameSet::operator== and vector::operator==
         if(verbose){
            std::cout << "Debug [BoardStateComparison]: Content mismatch after sorting sets." << std::endl;
            std::cout << "Actual board sets:" << std::endl; bs1.print();
            std::cout << "Expected board sets:" << std::endl; bs2.print();
        }
        return false;
    }
    return true;
}


void testBoardStructures() {
    TRACE_FUNCTION(); // Added TRACE_FUNCTION
    std::cout << "\n--- Testing Board Structures ---" << std::endl;

    Tile r1(1, red), r2(2, red), r3(3, red); // For run_set
    Tile r4_g(4, red), b4_g(4, blue), y4_g(4, yellow); // For a distinct group_set
    Tile r1_invalid(1,red), r3_invalid(3,red), r4_invalid(4,red); // For invalid run test
    Tile r3_dup_color_test(3,red), b3_dup_color_test(3,blue); // For invalid group test


    GameSet run_set({r1, r2, r3}, SetType::RUN);
    std::cout << "Testing Run: "; run_set.print();
    assert(run_set.isValid() && run_set.type == SetType::RUN && run_set.tiles.size() == 3);

    GameSet group_set({r4_g, b4_g, y4_g}, SetType::GROUP);
    std::cout << "Testing Group: "; group_set.print();
    assert(group_set.isValid() && group_set.type == SetType::GROUP);

    GameSet invalid_run_set({r1_invalid, r3_invalid, r4_invalid}, SetType::RUN);
    std::cout << "Testing Invalid Run (gap): "; invalid_run_set.print();
    assert(!invalid_run_set.isValid());

    GameSet invalid_group_set_dup_color({r3_dup_color_test, Tile(3,red) , b3_dup_color_test}, SetType::GROUP);
    std::cout << "Testing Invalid Group (duplicate color): "; invalid_group_set_dup_color.print();
    assert(!invalid_group_set_dup_color.isValid());

    BoardState board;
    std::cout << "\nInitial Board State:" << std::endl; board.print();
    board.addSet(run_set);
    board.addSet(group_set);
    std::cout << "\nBoard State after adding sets:" << std::endl; board.print();
    assert(board.isValidBoard()); // Corrected: tiles are now distinct

    std::vector<Tile> all_board_tiles = board.getAllTiles();
    std::cout << "All tiles on board (" << all_board_tiles.size() << "): ";
    for(const auto& t : all_board_tiles) { t.print(); std::cout << " "; }
    std::cout << std::endl;
    assert(all_board_tiles.size() == (run_set.tiles.size() + group_set.tiles.size()));

    GameSet run_set_copy(run_set.tiles, SetType::RUN);
    assert(run_set == run_set_copy);

    BoardState board_copy;
    board_copy.addSet(run_set_copy);
    board_copy.addSet(group_set);
    assert(board.sets.size() == 2 && board_copy.sets.size() == 2);
    assert(board == board_copy); // BoardState::operator== compares sorted sets internally if they are implemented that way or relies on vector order

    std::cout << "--- Board Structures Tests Passed ---" << std::endl;
}

void testSetFinder() {
    TRACE_FUNCTION(); // Added TRACE_FUNCTION
    std::cout << "\n--- Testing SetFinder ---" << std::endl;
    std::cout << "Starting testSetFinder..." << std::endl;

    Tile r1(1, red), r2(2, red), r3(3, red), r4(4, red), r5(5, red);
    Tile b1(1, blue), b2(2, blue), b3(3, blue), b4_b(4, blue); // Renamed to avoid conflict if original b4 was different
    Tile y3_tile(3, yellow), y5_tile(5, yellow); // Renamed
    Tile p3_tile(3, purple), p7_tile(7, purple); // Renamed

    std::vector<Tile> tc1_tiles = {r1, r2, r3};
    std::vector<GameSet> tc1_expected = {GameSet({r1, r2, r3}, SetType::RUN)};
    std::vector<GameSet> tc1_actual = SetFinder::find_all_possible_sets(tc1_tiles);
    assert(areGameSetVectorsEqual("TC1 Basic Run", tc1_actual, tc1_expected, true)); // Added verbose
    std::cout << "TC1 Basic Run: Passed" << std::endl;

    std::vector<Tile> tc2_tiles = {b1, b2, b3, b4_b};
    std::vector<GameSet> tc2_expected = {
        GameSet({b1, b2, b3}, SetType::RUN),
        GameSet({b1, b2, b3, b4_b}, SetType::RUN),
        GameSet({b2, b3, b4_b}, SetType::RUN)
    };
    std::vector<GameSet> tc2_actual = SetFinder::find_all_possible_sets(tc2_tiles);
    assert(areGameSetVectorsEqual("TC2 Multiple Runs", tc2_actual, tc2_expected, true));
    std::cout << "TC2 Multiple Runs: Passed" << std::endl;

    std::vector<Tile> tc3_tiles = {r3, b3, y3_tile};
    std::vector<GameSet> tc3_expected = {GameSet({b3, r3, y3_tile}, SetType::GROUP)};
    std::vector<GameSet> tc3_actual = SetFinder::find_all_possible_sets(tc3_tiles);
    assert(areGameSetVectorsEqual("TC3 Basic Group", tc3_actual, tc3_expected, true));
    std::cout << "TC3 Basic Group: Passed" << std::endl;

    Tile r7(7, red), b7_tile(7, blue), y7_tile(7, yellow); // Renamed
    std::vector<Tile> tc4_tiles = {r7, b7_tile, y7_tile, p7_tile};
    std::vector<GameSet> tc4_expected = {
        GameSet({b7_tile, p7_tile, r7}, SetType::GROUP), GameSet({b7_tile, p7_tile, y7_tile}, SetType::GROUP),
        GameSet({b7_tile, r7, y7_tile}, SetType::GROUP), GameSet({p7_tile, r7, y7_tile}, SetType::GROUP),
        GameSet({b7_tile, p7_tile, r7, y7_tile}, SetType::GROUP) };
    std::vector<GameSet> tc4_actual = SetFinder::find_all_possible_sets(tc4_tiles);
    assert(areGameSetVectorsEqual("TC4 Multiple Groups", tc4_actual, tc4_expected, true));
    std::cout << "TC4 Multiple Groups: Passed" << std::endl;

    std::vector<Tile> tc5_tiles = {r1, r2, r3, b3, y3_tile};
    std::vector<GameSet> tc5_expected = {
        GameSet({r1, r2, r3}, SetType::RUN), GameSet({b3, r3, y3_tile}, SetType::GROUP)};
    std::vector<GameSet> tc5_actual = SetFinder::find_all_possible_sets(tc5_tiles);
    assert(areGameSetVectorsEqual("TC5 Mixed Run/Group", tc5_actual, tc5_expected, true));
    std::cout << "TC5 Mixed Run/Group: Passed" << std::endl;

    // Other SetFinder tests (6, 6b, 7, 8) from original are preserved below
    Tile r1_dup(1, red);
    std::vector<Tile> tc6_tiles = {r1, r1_dup, r2, r3, b1, y3_tile};
    std::vector<GameSet> tc6_expected = { GameSet({r1, r2, r3}, SetType::RUN) };
    std::vector<GameSet> tc6_actual = SetFinder::find_all_possible_sets(tc6_tiles);
    assert(areGameSetVectorsEqual("TC6 Duplicates (Run)", tc6_actual, tc6_expected, true));
    std::cout << "TC6 Duplicates (Run): Passed" << std::endl;

    Tile b1_dup(1, blue); Tile y1_for_group(1, yellow);
    std::vector<Tile> tc6b_tiles = {r1, r1_dup, b1, b1_dup, y1_for_group};
    std::vector<GameSet> tc6b_expected = { GameSet({b1, r1, y1_for_group}, SetType::GROUP) };
    std::vector<GameSet> tc6b_actual = SetFinder::find_all_possible_sets(tc6b_tiles);
    assert(areGameSetVectorsEqual("TC6b Duplicates (Group)", tc6b_actual, tc6b_expected, true));
    std::cout << "TC6b Duplicates (Group): Passed" << std::endl;

    std::vector<Tile> tc7_tiles = {r1, b2, y5_tile};
    std::vector<GameSet> tc7_expected = {};
    std::vector<GameSet> tc7_actual = SetFinder::find_all_possible_sets(tc7_tiles);
    assert(areGameSetVectorsEqual("TC7 No Sets", tc7_actual, tc7_expected, true));
    std::cout << "TC7 No Sets: Passed" << std::endl;

    std::vector<Tile> tc8_tiles = {r1, r2};
    std::vector<GameSet> tc8_expected = {};
    std::vector<GameSet> tc8_actual = SetFinder::find_all_possible_sets(tc8_tiles);
    assert(areGameSetVectorsEqual("TC8 Two Tiles", tc8_actual, tc8_expected, true));
    std::cout << "TC8 Two Tiles: Passed" << std::endl;

    std::cout << "--- SetFinder Tests Passed ---" << std::endl;
}

void testIsValidBoard() { // This is the original testIsValidBoard
    TRACE_FUNCTION();
    std::cout << "\n--- Testing BoardState::isValidBoard ---" << std::endl;

    Tile r1(1,red), r2(2,red), r3(3,red), r4_r(4,red); // Renamed r4
    Tile b4_b(4,blue), y4_y(4,yellow); // Renamed color specific tiles
    Tile p3_p(3,purple); // Renamed

    GameSet valid_run({r1,r2,r3}, SetType::RUN);
    GameSet valid_group({r4_r, b4_b, y4_y}, SetType::GROUP);
    GameSet invalid_run({r1,r3,r4_r}, SetType::RUN);
    GameSet group_with_r1({r1, Tile(1,blue), Tile(1,yellow)}, SetType::GROUP);

    BoardState board1; assert(board1.isValidBoard()); std::cout << "TC1 Empty Board: Passed" << std::endl;
    BoardState board2; board2.addSet(valid_run); board2.addSet(valid_group);
    assert(board2.isValidBoard()); std::cout << "TC2 Valid Board: Passed" << std::endl;

    std::vector<GameSet> sets_for_board3 = {valid_run, invalid_run};
    BoardState board3(sets_for_board3);
    assert(!board3.isValidBoard()); std::cout << "TC3 Invalid Set on Board: Passed" << std::endl;

    std::vector<GameSet> sets_for_board4 = {valid_run, group_with_r1}; // r1 duplicated
    BoardState board4(sets_for_board4);
    assert(valid_run.isValid() && group_with_r1.isValid()); // Individual sets are fine
    assert(!board4.isValidBoard()); std::cout << "TC4 Tile Duplication Across Sets: Passed" << std::endl;

    GameSet valid_run_2({Tile(5,blue), Tile(6,blue), Tile(7,blue)}, SetType::RUN);
    BoardState board5; board5.addSet(valid_run); board5.addSet(valid_group); board5.addSet(valid_run_2);
    assert(board5.isValidBoard()); std::cout << "TC5 Valid Board Multiple Sets: Passed" << std::endl;

    std::cout << "--- BoardState::isValidBoard Tests Passed ---" << std::endl;
}

void testIsBoardValidFunction() { // This is the original testIsBoardValidFunction
    TRACE_FUNCTION();
    std::cout << "\n--- Testing is_board_valid (free function) ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red), r4_r(4,red); // Renamed r4
    Tile b4_b(4,blue), y4_y(4,yellow); // Renamed
    Tile p3_tile(3, purple);

    GameSet valid_run({r1,r2,r3}, SetType::RUN);
    GameSet valid_group({r4_r,b4_b,y4_y}, SetType::GROUP);
    GameSet invalid_set_short_run({r1,r2}, SetType::RUN);
    GameSet group_with_r1({r1, Tile(1,blue), Tile(1,yellow)}, SetType::GROUP);

    std::vector<GameSet> empty_sets;
    assert(is_board_valid(empty_sets)); std::cout << "TC1 Empty vector of sets: Passed" << std::endl;
    std::vector<GameSet> valid_board_sets = {valid_run, valid_group};
    assert(is_board_valid(valid_board_sets)); std::cout << "TC2 All valid sets, unique tiles: Passed" << std::endl;
    std::vector<GameSet> board_with_invalid_set = {valid_run, invalid_set_short_run};
    assert(!is_board_valid(board_with_invalid_set)); std::cout << "TC3 Contains an invalid set: Passed" << std::endl;
    std::vector<GameSet> board_with_duplicate_tiles = {valid_run, group_with_r1}; // r1 duplicated
    assert(valid_run.isValid() && group_with_r1.isValid());
    assert(!is_board_valid(board_with_duplicate_tiles)); std::cout << "TC4 Valid sets, but duplicate tiles: Passed" << std::endl;
    std::vector<GameSet> single_valid_set = {valid_run};
    assert(is_board_valid(single_valid_set)); std::cout << "TC5 Single valid set: Passed" << std::endl;
    std::vector<GameSet> single_invalid_set = {invalid_set_short_run};
    assert(!is_board_valid(single_invalid_set)); std::cout << "TC6 Single invalid set: Passed" << std::endl;

    std::cout << "--- is_board_valid (free function) Tests Passed ---" << std::endl;
}

void testCanAddTilesToBoard() { // This is the original, now updated for std::optional
    TRACE_FUNCTION();
    std::cout << "\n--- Testing can_add_tiles_to_board ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red), r5(5,red), r6(6,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue), b4_b(4,blue); // Renamed b4
    Tile y1(1,yellow), y2(2,yellow), y3_y(3,yellow), y4_y(4,yellow), y5_y(5,yellow); // Renamed
    Tile p1(1,purple), p2(2,purple), p3_p(3,purple), p4_p(4,purple); // Renamed

    // TC1: Empty board, add valid set
    BoardState empty_board; std::vector<Tile> add1 = {r1,r2,r3};
    std::optional<BoardState> res1 = BoardManipulation::can_add_tiles_to_board(empty_board,add1);
    BoardState exp1; exp1.addSet(GameSet(add1,SetType::RUN));
    assert(res1.has_value() && areBoardStatesEquivalent(*res1, exp1, true));
    std::cout << "TC1 Empty board, add valid set: Passed" << std::endl;

    // TC2: Empty board, add invalid set (cannot form)
    std::vector<Tile> add2 = {r1,r2};
    std::optional<BoardState> res2 = BoardManipulation::can_add_tiles_to_board(empty_board,add2);
    assert(!res2.has_value());
    std::cout << "TC2 Empty board, add invalid set: Passed" << std::endl;

    // TC3: Existing, add separate valid set
    BoardState board3; board3.addSet(GameSet({b1,b2,b3},SetType::RUN));
    std::vector<Tile> add3 = {r1,r2,r3};
    std::optional<BoardState> res3 = BoardManipulation::can_add_tiles_to_board(board3,add3);
    BoardState exp3; exp3.addSet(GameSet({b1,b2,b3},SetType::RUN)); exp3.addSet(GameSet({r1,r2,r3},SetType::RUN));
    assert(res3.has_value() && areBoardStatesEquivalent(*res3,exp3,true));
    std::cout << "TC3 Existing board, add separate valid set: Passed" << std::endl;

    // TC4: Existing, add tiles that cannot be placed
    BoardState board4; board4.addSet(GameSet({b1,b2,b3},SetType::RUN));
    std::vector<Tile> add4 = {r1,r2};
    std::optional<BoardState> res4 = BoardManipulation::can_add_tiles_to_board(board4,add4);
    assert(!res4.has_value());
    std::cout << "TC4 Existing board, add tiles that cannot be placed: Passed" << std::endl;

    // TC5: Add tile that extends run
    BoardState board5; board5.addSet(GameSet({r1,r2,r3},SetType::RUN)); board5.addSet(GameSet({b1,b2,b3},SetType::RUN));
    std::vector<Tile> add5 = {r4};
    std::optional<BoardState> res5 = BoardManipulation::can_add_tiles_to_board(board5,add5);
    BoardState exp5; exp5.addSet(GameSet({r1,r2,r3,r4},SetType::RUN)); exp5.addSet(GameSet({b1,b2,b3},SetType::RUN));
    assert(res5.has_value() && areBoardStatesEquivalent(*res5,exp5,true));
    std::cout << "TC5 Add tile that extends an existing run (via rebuild): Passed" << std::endl;

    // TC6: Add tiles requiring break/reform
    BoardState board6; board6.addSet(GameSet({r1,r2,r3,r4},SetType::RUN)); board6.addSet(GameSet({y5_y,Tile(6,yellow),Tile(7,yellow)},SetType::RUN));
    std::vector<Tile> add6 = {b4_b,p4_p}; // Add B4, P4 to use R4 from board
    std::optional<BoardState> res6 = BoardManipulation::can_add_tiles_to_board(board6,add6);
    BoardState exp6; exp6.addSet(GameSet({r1,r2,r3},SetType::RUN)); exp6.addSet(GameSet({r4,b4_b,p4_p},SetType::GROUP)); exp6.addSet(GameSet({y5_y,Tile(6,yellow),Tile(7,yellow)},SetType::RUN));
    assert(res6.has_value() && areBoardStatesEquivalent(*res6,exp6,true));
    std::cout << "TC6 Add tiles requiring break and reform: Passed" << std::endl;

    // TC7: Tiles cannot be legally added (partial set)
    BoardState board7; board7.addSet(GameSet({r1,r2,r3},SetType::RUN));
    std::vector<Tile> add7 = {b1,b2};
    std::optional<BoardState> res7 = BoardManipulation::can_add_tiles_to_board(board7,add7);
    assert(!res7.has_value());
    std::cout << "TC7 Add tiles that cannot be legally placed (partial set): Passed" << std::endl;

    // TC8: Empty tiles_to_add
    BoardState board8; board8.addSet(GameSet({r1,r2,r3},SetType::RUN));
    std::vector<Tile> add8 = {};
    std::optional<BoardState> res8 = BoardManipulation::can_add_tiles_to_board(board8,add8);
    assert(!res8.has_value()); // Empty tiles_to_add now returns nullopt
    std::cout << "TC8 Empty tiles_to_add: Passed" << std::endl;

    // TC9: Added tile must be used but cannot complete board
    BoardState board9; board9.addSet(GameSet({r1,r2,r3},SetType::RUN));
    std::vector<Tile> add9 = {b4_b};
    std::optional<BoardState> res9 = BoardManipulation::can_add_tiles_to_board(board9,add9);
    assert(!res9.has_value());
    std::cout << "TC9 Added tile must be used but cannot complete a board: Passed" << std::endl;
    std::cout << "--- can_add_tiles_to_board Tests Passed (Initial Set from original) ---" << std::endl;

    // Original Heavyweight tests for can_add_tiles_to_board
    std::cout << "\n--- Testing can_add_tiles_to_board (Heavyweight Cases from original) ---" << std::endl;
    Tile r7_h(7,red), r8_h(8,red), r9_h(9,red), r10_h(10,red), r11_h(11,red), r12_h(12,red), r13_h(13,red);
    Tile b5_h(5,blue), b6_h(6,blue), b7_h(7,blue), b8_h(8,blue), b9_h(9,blue), b10_h(10,blue);
    Tile y1_h(1,yellow), y2_h(2,yellow), y3_h(3,yellow), y4_h(4,yellow); // Define y1_h etc.
    Tile y5_h(5,yellow), y6_h(6,yellow), y7_h(7,yellow);
    Tile p1_h(1,purple), p2_h(2,purple), p3_h(3,purple), p4_h(4,purple), p13_h(13,purple);

    // HW1
    BoardState bhw1_curr;
    bhw1_curr.addSet(GameSet({r1,r2,r3},SetType::RUN)); bhw1_curr.addSet(GameSet({r4,r5,r6},SetType::RUN));
    bhw1_curr.addSet(GameSet({b1,b2,b3},SetType::RUN)); bhw1_curr.addSet(GameSet({y1_h,y2_h,y3_h},SetType::RUN));
    bhw1_curr.addSet(GameSet({p1_h,p2_h,p3_h},SetType::RUN));
    std::vector<Tile> add_hw1 = {r7_h, b4_b, y4_h};
    std::optional<BoardState> res_hw1 = BoardManipulation::can_add_tiles_to_board(bhw1_curr,add_hw1);
    BoardState exp_hw1;
    exp_hw1.addSet(GameSet({b1,b2,b3},SetType::RUN)); exp_hw1.addSet(GameSet({p1_h,p2_h,p3_h},SetType::RUN));
    exp_hw1.addSet(GameSet({r1,r2,r3},SetType::RUN)); exp_hw1.addSet(GameSet({r5,r6,r7_h},SetType::RUN));
    exp_hw1.addSet(GameSet({y1_h,y2_h,y3_h},SetType::RUN)); exp_hw1.addSet(GameSet({b4_b,r4,y4_h},SetType::GROUP));
    assert(res_hw1.has_value() && areBoardStatesEquivalent(*res_hw1,exp_hw1,true));
    std::cout << "TC_HW1: Passed" << std::endl;

    // HW2
    BoardState bhw2_curr;
    bhw2_curr.addSet(GameSet({r1,r2,r3,r4},SetType::RUN)); bhw2_curr.addSet(GameSet({b1,b2,b3,b4_b},SetType::RUN));
    bhw2_curr.addSet(GameSet({y1_h,y2_h,y3_h,y4_h},SetType::RUN)); bhw2_curr.addSet(GameSet({p1_h,p2_h,p3_h,p4_p},SetType::RUN));
    std::vector<Tile> add_hw2 = {r10_h,b10_h,p13_h};
    std::optional<BoardState> res_hw2 = BoardManipulation::can_add_tiles_to_board(bhw2_curr,add_hw2);
    assert(!res_hw2.has_value());
    std::cout << "TC_HW2: Passed" << std::endl;

    // HW3
    BoardState bhw3_curr;
    bhw3_curr.addSet(GameSet({r1,r2,r3,r4,r5,r6},SetType::RUN)); bhw3_curr.addSet(GameSet({b1,b2,b3,b4_b},SetType::RUN));
    std::vector<Tile> add_hw3 = {y4_h,p4_p};
    std::optional<BoardState> res_hw3 = BoardManipulation::can_add_tiles_to_board(bhw3_curr,add_hw3);
    BoardState exp_hw3;
    exp_hw3.addSet(GameSet({b1,b2,b3},SetType::RUN)); exp_hw3.addSet(GameSet({r1,r2,r3},SetType::RUN));
    exp_hw3.addSet(GameSet({r4,r5,r6},SetType::RUN)); exp_hw3.addSet(GameSet({b4_b,p4_p,y4_h},SetType::GROUP));
    assert(res_hw3.has_value() && areBoardStatesEquivalent(*res_hw3,exp_hw3,true));
    std::cout << "TC_HW3: Passed" << std::endl;

    // HW4
    BoardState bhw4_curr;
    bhw4_curr.addSet(GameSet({r1,r2,r3,r4,r5},SetType::RUN)); bhw4_curr.addSet(GameSet({b1,b2,b3,b4_b,b5_h},SetType::RUN));
    std::vector<Tile> add_hw4 = {y4_h,p4_p};
    std::optional<BoardState> res_hw4 = BoardManipulation::can_add_tiles_to_board(bhw4_curr,add_hw4);
    assert(!res_hw4.has_value());
    std::cout << "TC_HW4: Passed" << std::endl;

    // HW5
    BoardState bhw5_curr;
    bhw5_curr.addSet(GameSet({r1,r2,r3},SetType::RUN)); bhw5_curr.addSet(GameSet({r4,r5,r6},SetType::RUN));
    bhw5_curr.addSet(GameSet({r7_h,r8_h,r9_h},SetType::RUN)); bhw5_curr.addSet(GameSet({b1,b2,b3},SetType::RUN));
    bhw5_curr.addSet(GameSet({b4_b,b5_h,b6_h},SetType::RUN)); bhw5_curr.addSet(GameSet({y1_h,y2_h,y3_h,y4_h},SetType::RUN));
    bhw5_curr.addSet(GameSet({p1_h,p2_h,p3_h},SetType::RUN));
    std::vector<Tile> add_hw5 = {r10_h,r11_h,r12_h, b7_h,b8_h,b9_h, y5_h,y6_h,y7_h};
    std::optional<BoardState> res_hw5 = BoardManipulation::can_add_tiles_to_board(bhw5_curr,add_hw5);
    BoardState exp_hw5;
    exp_hw5.addSet(GameSet({b1,b2,b3},SetType::RUN));exp_hw5.addSet(GameSet({b4_b,b5_h,b6_h},SetType::RUN));
    exp_hw5.addSet(GameSet({b7_h,b8_h,b9_h},SetType::RUN));exp_hw5.addSet(GameSet({p1_h,p2_h,p3_h},SetType::RUN));
    exp_hw5.addSet(GameSet({r1,r2,r3},SetType::RUN));exp_hw5.addSet(GameSet({r4,r5,r6},SetType::RUN));
    exp_hw5.addSet(GameSet({r7_h,r8_h,r9_h},SetType::RUN));exp_hw5.addSet(GameSet({r10_h,r11_h,r12_h},SetType::RUN));
    exp_hw5.addSet(GameSet({y1_h,y2_h,y3_h},SetType::RUN));exp_hw5.addSet(GameSet({y4_h,y5_h,y6_h,y7_h},SetType::RUN));
    assert(res_hw5.has_value() && areBoardStatesEquivalent(*res_hw5,exp_hw5,true));
    std::cout << "TC_HW5: Passed" << std::endl;

    // HW6
    BoardState bhw6_curr;
    bhw6_curr.addSet(GameSet({r1,r2,r3},SetType::RUN)); bhw6_curr.addSet(GameSet({b1,b2,b3},SetType::RUN));
    std::vector<Tile> add_hw6 = {r4,b4_b,r5};
    std::optional<BoardState> res_hw6 = BoardManipulation::can_add_tiles_to_board(bhw6_curr,add_hw6);
    BoardState exp_hw6; exp_hw6.addSet(GameSet({r1,r2,r3,r4,r5},SetType::RUN)); exp_hw6.addSet(GameSet({b1,b2,b3,b4_b},SetType::RUN));
    assert(res_hw6.has_value() && areBoardStatesEquivalent(*res_hw6,exp_hw6,true));
    std::cout << "TC_HW6: Passed" << std::endl;
    std::cout << "--- can_add_tiles_to_board (Heavyweight Cases) Tests Passed ---" << std::endl;
}

// Newly added test suite for find_best_move
void testFindBestMove() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing MoveFinder::find_best_move ---" << std::endl;

    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red), r5(5,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue), b4_b(4,blue), b5_b(5,blue), b6_b(6,blue), b7_b(7,blue), b8_b(8,blue);
    Tile y1_y(1,yellow), y2_y(2,yellow), y3_y(3,yellow); // Renamed yellow tiles
    Tile y10(10,yellow), y11(11,yellow), y12(12,yellow);
    Tile k5_tile(5, 0);
    Tile k6_tile(6,0);

    // TC1: Simple play all
    BoardState cb1; std::vector<Tile> hand1 = {r1, r2, r3};
    std::optional<Move> move1 = MoveFinder::find_best_move(cb1, hand1);
    assert(move1.has_value() && move1->tiles_played_count == 3 && move1->remaining_hand.empty());
    if(move1){BoardState exp; exp.addSet(GameSet({r1,r2,r3},SetType::RUN)); assert(areBoardStatesEquivalent(move1->new_board_state,exp,true));}
    std::cout << "find_best_move TC1 (Simple play all): Passed" << std::endl;

    // TC2: No move
    BoardState cb2; std::vector<Tile> hand2 = {r1, r2};
    std::optional<Move> move2 = MoveFinder::find_best_move(cb2, hand2);
    assert(!move2.has_value());
    std::cout << "find_best_move TC2 (No move): Passed" << std::endl;

    // TC3: Plays more tiles (actually plays all)
    BoardState cb3; std::vector<Tile> hand3 = {r1,r2,r3, b5_b,b6_b,b7_b,b8_b};
    std::optional<Move> move3 = MoveFinder::find_best_move(cb3, hand3);
    assert(move3.has_value() && move3->tiles_played_count == 7 && move3->remaining_hand.empty());
    if(move3){BoardState exp; exp.addSet(GameSet({r1,r2,r3},SetType::RUN)); exp.addSet(GameSet({b5_b,b6_b,b7_b,b8_b},SetType::RUN)); assert(areBoardStatesEquivalent(move3->new_board_state,exp,true));}
    std::cout << "find_best_move TC3 (Plays more tiles - actually plays all): Passed" << std::endl;

    // TC4: Few board, many hand (actually plays all from hand)
    BoardState cb4; cb4.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    std::vector<Tile> hand4 = {r4, r5, b1,b2,b3};
    std::optional<Move> move4 = MoveFinder::find_best_move(cb4, hand4);
    assert(move4.has_value() && move4->tiles_played_count == 5 && move4->remaining_hand.empty());
    if(move4){BoardState exp; exp.addSet(GameSet({r1,r2,r3,r4,r5},SetType::RUN)); exp.addSet(GameSet({b1,b2,b3},SetType::RUN)); assert(areBoardStatesEquivalent(move4->new_board_state,exp,true));}
    std::cout << "find_best_move TC4 (Few board, many hand - actually plays all from hand): Passed" << std::endl;

    // TC5: Many board, few hand
    BoardState cb5; cb5.addSet(GameSet({r1,r2,r3},SetType::RUN)); cb5.addSet(GameSet({b1,b2,b3},SetType::RUN)); cb5.addSet(GameSet({y1_y,y2_y,y3_y},SetType::RUN));
    std::vector<Tile> hand5 = {r4, k5_tile, k6_tile};
    std::optional<Move> move5 = MoveFinder::find_best_move(cb5, hand5);
    assert(move5.has_value() && move5->tiles_played_count == 1 && sorted(move5->remaining_hand) == sorted({k5_tile,k6_tile}));
    if(move5){BoardState exp; exp.addSet(GameSet({r1,r2,r3,r4},SetType::RUN)); exp.addSet(GameSet({b1,b2,b3},SetType::RUN)); exp.addSet(GameSet({y1_y,y2_y,y3_y},SetType::RUN)); assert(areBoardStatesEquivalent(move5->new_board_state,exp,true));}
    std::cout << "find_best_move TC5 (Many board, few hand): Passed" << std::endl;

    // TC6: Many board, many hand - complex
    BoardState cb6; cb6.addSet(GameSet({r1,r2,r3},SetType::RUN)); cb6.addSet(GameSet({b5_b,b6_b,b7_b},SetType::RUN));
    std::vector<Tile> hand6 = {r4, b4_b, y10,y11,y12};
    std::optional<Move> move6 = MoveFinder::find_best_move(cb6, hand6);
    assert(move6.has_value() && move6->tiles_played_count == 5 && move6->remaining_hand.empty());
    if(move6){BoardState exp; exp.addSet(GameSet({r1,r2,r3,r4},SetType::RUN)); exp.addSet(GameSet({b4_b,b5_b,b6_b,b7_b},SetType::RUN)); exp.addSet(GameSet({y10,y11,y12},SetType::RUN)); assert(areBoardStatesEquivalent(move6->new_board_state,exp,true));}
    std::cout << "find_best_move TC6 (Many board, many hand - complex): Passed" << std::endl;
    std::cout << "--- MoveFinder::find_best_move ALL CASES PASSED ---" << std::endl;
}


// Original main function modified to include all tests
int main() {
	assert( allTiles.size() == 104 );
	std::cout << "Initial tile generation test passed." << std::endl;

	testBoardStructures();
    std::cout << "\nAttempting to run SetFinder tests..." << std::endl;
	testSetFinder();
    std::cout << "\nAttempting to run isValidBoard tests..." << std::endl;
    testIsValidBoard();
    std::cout << "\nAttempting to run is_board_valid (free function) tests..." << std::endl;
    testIsBoardValidFunction();
    std::cout << "\nAttempting to run can_add_tiles_to_board tests..." << std::endl;
    testCanAddTilesToBoard(); // This now uses std::optional and its assertions are updated.

    testFindBestMove(); // Added call to new test suite

#ifdef ENABLE_PERFORMANCE_TRACING
    PerformanceTracer::print_performance_report();
#else
    std::cout << "\nPerformance tracing not enabled (define TRACE=1 with make)." << std::endl;
#endif

	return 0;
}
