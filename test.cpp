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
#include "SetFinder.hpp"
#include "MoveFinder.hpp" // Added for find_best_move tests
#include "GameTypes.hpp"  // For Move struct, Tile, GameSet etc.

#include <cassert>
#include <iostream>
#include <algorithm> // For std::sort
#include <vector>    // For std::vector
#include <set>       // For std::set in test comparisons
#include <optional>  // For std::optional

// Existing global variable from original test file structure
// vector<Tile> allTiles = generateAllTiles(); // Can be used if needed, or define tiles locally.

// Helper to sort a vector of tiles (useful for comparing hands)
std::vector<Tile> sorted(std::vector<Tile> tiles) {
    std::sort(tiles.begin(), tiles.end());
    return tiles;
}

// Helper to compare vectors of GameSet for testing purposes
// Now uses a boolean for verbosity control.
bool areGameSetVectorsEqual(const std::string& tc_name, std::vector<GameSet> v1, std::vector<GameSet> v2, bool verbose = false) {
    std::sort(v1.begin(), v1.end());
    std::sort(v2.begin(), v2.end());
    if (v1.size() != v2.size() || v1 != v2) {
        if (verbose) {
            std::cout << "Debug [" << tc_name << "]: GameSet vectors not equal." << std::endl;
            std::cout << "Actual (size " << v1.size() << "):" << std::endl; for(const auto& s : v1) s.print();
            std::cout << "Expected (size " << v2.size() << "):" << std::endl; for(const auto& s : v2) s.print();
        }
        return false;
    }
    return true;
}

// Helper to compare BoardState objects by sorting their sets
bool areBoardStatesEquivalent(BoardState bs1, BoardState bs2, bool verbose = false) {
    // It's often good to ensure boards are valid before comparing, if that's an expectation.
    // if (!bs1.isValidBoard() || !bs2.isValidBoard()) {
    //     if (verbose) std::cout << "Warning: Comparing potentially invalid board states for equivalence." << std::endl;
    // }
    return areGameSetVectorsEqual("BoardStateComparison", bs1.sets, bs2.sets, verbose);
}


void testCoreGameLogicAndStructures() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing Core Game Logic & Structures ---" << std::endl;
    Tile r1(1, red), r2(2, red), r3(3, red), r4(4, red);
    Tile b3(3, blue), y3(3, yellow);
    Tile r4_g(4,red), b4_g(4,blue), y4_g(4,yellow); // Distinct tiles for a group

    GameSet run_set({r1, r2, r3}, SetType::RUN); // R1, R2, R3
    assert(run_set.isValid());
    GameSet group_set({r4_g, b4_g, y4_g}, SetType::GROUP); // R4, B4, Y4 (distinct from run_set)
    assert(group_set.isValid());

    BoardState board;
    board.addSet(run_set);
    board.addSet(group_set);
    assert(board.isValidBoard()); // Should now be true as sets use distinct tiles
    assert(board.getAllTiles().size() == 6);


    std::cout << "Core Game Logic & Structures Tests Passed." << std::endl;
}

void testSetFinderFunctionality() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing SetFinder ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red);
    Tile b3(3,blue), y3(3,yellow);

    std::vector<Tile> tiles_for_setfinder = {r1, r2, r3, b3, y3};
    std::vector<GameSet> expected_sets = {
        GameSet({r1,r2,r3}, SetType::RUN),
        GameSet({b3,r3,y3}, SetType::GROUP) // GameSet sorts tiles: b3, r3, y3
    };
    std::vector<GameSet> actual_sets = SetFinder::find_all_possible_sets(tiles_for_setfinder);
    assert(areGameSetVectorsEqual("SetFinder Mixed", actual_sets, expected_sets, true));
    std::cout << "SetFinder Tests Passed." << std::endl;
}

void testBoardValidityChecks() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing Board Validity ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red);
    GameSet valid_run({r1,r2,r3}, SetType::RUN);
    GameSet group_with_r1({r1, Tile(1,blue), Tile(1,yellow)}, SetType::GROUP); // R1, B1, Y1

    BoardState b_valid; b_valid.addSet(valid_run);
    assert(b_valid.isValidBoard());
    assert(is_board_valid(b_valid.sets));

    // Construct board with duplicate tile R1 across sets
    std::vector<GameSet> sets_for_invalid_board = {
        GameSet({r1,r2,r3}, SetType::RUN),
        GameSet({r1, Tile(1,blue), Tile(1,yellow)}, SetType::GROUP)
    };
    BoardState b_invalid_dup(sets_for_invalid_board);
    assert(!b_invalid_dup.isValidBoard()); // isValidBoard should detect duplication
    assert(!is_board_valid(b_invalid_dup.sets)); // Free function should also detect
    std::cout << "Board Validity Tests Passed." << std::endl;
}

void testCanAddTilesToBoardFunctionality() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing BoardManipulation::can_add_tiles_to_board ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue);
    Tile y4(4,yellow), p4(4,purple);

    BoardState empty_board;
    std::vector<Tile> tiles_to_add_1 = {r1, r2, r3};
    std::optional<BoardState> res1 = BoardManipulation::can_add_tiles_to_board(empty_board, tiles_to_add_1);
    assert(res1.has_value());
    if(res1) {
        assert(res1->getAllTiles().size() == 3 && res1->isValidBoard());
        BoardState expected_b; expected_b.addSet(GameSet({r1,r2,r3},SetType::RUN));
        assert(areBoardStatesEquivalent(*res1, expected_b));
    }

    BoardState board_existing; board_existing.addSet(GameSet({b1,b2,b3}, SetType::RUN));
    std::vector<Tile> tiles_to_add_2 = {r4, y4, p4};
    std::optional<BoardState> res2 = BoardManipulation::can_add_tiles_to_board(board_existing, tiles_to_add_2);
    assert(res2.has_value());
    if(res2) {
        assert(res2->getAllTiles().size() == 6 && res2->isValidBoard());
        BoardState expected_b;
        expected_b.addSet(GameSet({b1,b2,b3},SetType::RUN));
        expected_b.addSet(GameSet({p4,r4,y4},SetType::GROUP)); // GameSet sorts tiles
        assert(areBoardStatesEquivalent(*res2, expected_b, true));
    }

    std::vector<Tile> tiles_cannot_add = {Tile(10,red), Tile(11,red)}; // Cannot form a set alone or with board
    std::optional<BoardState> res3 = BoardManipulation::can_add_tiles_to_board(board_existing, tiles_cannot_add);
    assert(!res3.has_value());

    std::cout << "can_add_tiles_to_board Tests Passed." << std::endl;
}

void testFindBestMove() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing MoveFinder::find_best_move ---" << std::endl;

    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red), r5(5,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue), b4(4,blue), b5(5,blue), b6(6,blue), b7(7,blue), b8(8,blue);
    Tile y1(1,yellow), y2(2,yellow), y3(3,yellow);
    Tile y10(10,yellow), y11(11,yellow), y12(12,yellow);
    Tile p4_tile(4,purple); // Explicitly named to avoid confusion with just 'p4'
    Tile k5_tile(5, 0); // Assuming color 0 for black/generic if needed for a distinct tile
    Tile k6_tile(6,0);  // Same here

    // Test Case 1: Simple move possible (play all tiles in hand to form a new set)
    BoardState cb1; // Empty board
    std::vector<Tile> hand1 = {r1, r2, r3};
    std::optional<Move> move1 = MoveFinder::find_best_move(cb1, hand1);
    assert(move1.has_value());
    if (move1) {
        assert(move1->tiles_played_count == 3);
        assert(move1->remaining_hand.empty());
        assert(move1->new_board_state.isValidBoard());
        assert(move1->new_board_state.getAllTiles().size() == 3);
        BoardState expected_b; expected_b.addSet(GameSet({r1,r2,r3}, SetType::RUN));
        assert(areBoardStatesEquivalent(move1->new_board_state, expected_b));
    }
    std::cout << "find_best_move TC1 (Simple play all): Passed" << std::endl;

    // Test Case 2: No possible move
    BoardState cb2; // Empty board
    std::vector<Tile> hand2 = {r1, r2}; // Cannot form a set
    std::optional<Move> move2 = MoveFinder::find_best_move(cb2, hand2);
    assert(!move2.has_value());
    std::cout << "find_best_move TC2 (No move): Passed" << std::endl;

    // Test Case 3: Chooses move that plays more tiles
    BoardState cb3; // Empty board
    std::vector<Tile> hand3 = {r1,r2,r3, b5,b6,b7,b8}; // R1,R2,R3 (plays 3) and B5,B6,B7,B8 (plays 4)
    std::optional<Move> move3 = MoveFinder::find_best_move(cb3, hand3);
    assert(move3.has_value());
    if (move3) {
        // All 7 tiles can be played by forming two sets: {R1,R2,R3} and {B5,B6,B7,B8}
        assert(move3->tiles_played_count == 7);
        assert(move3->remaining_hand.empty());

        BoardState expected_b;
        expected_b.addSet(GameSet({r1,r2,r3}, SetType::RUN));
        expected_b.addSet(GameSet({b5,b6,b7,b8}, SetType::RUN));
        assert(areBoardStatesEquivalent(move3->new_board_state, expected_b, true));
    }
    std::cout << "find_best_move TC3 (Plays more tiles - actually plays all): Passed" << std::endl;

    // Test Case 4: Few tiles on board, many in hand
    BoardState cb4; cb4.addSet(GameSet({r1,r2,r3}, SetType::RUN)); // Board: R1,R2,R3
    std::vector<Tile> hand4 = {r4, r5, b1,b2,b3}; // Hand: R4,R5 (can play 2 to extend R-run) or B1,B2,B3 (can play 3 for new B-run)
    std::optional<Move> move4 = MoveFinder::find_best_move(cb4, hand4);
    assert(move4.has_value());
    if (move4) {
        // All 5 tiles from hand can be played.
        // Board: {R1,R2,R3} + Hand: {R4,R5,B1,B2,B3} -> New Board: {R1,R2,R3,R4,R5}, {B1,B2,B3}
        assert(move4->tiles_played_count == 5);
        assert(move4->remaining_hand.empty());
        assert(move4->new_board_state.getAllTiles().size() == 8); // 3 original + 5 played
        BoardState expected_b;
        expected_b.addSet(GameSet({r1,r2,r3,r4,r5}, SetType::RUN));
        expected_b.addSet(GameSet({b1,b2,b3}, SetType::RUN));
        assert(areBoardStatesEquivalent(move4->new_board_state, expected_b, true));
    }
    std::cout << "find_best_move TC4 (Few board, many hand - actually plays all from hand): Passed" << std::endl;

    // Test Case 5: Many tiles on board, few in hand
    BoardState cb5;
    cb5.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    cb5.addSet(GameSet({b1,b2,b3}, SetType::RUN));
    cb5.addSet(GameSet({y1,y2,y3}, SetType::RUN));
    std::vector<Tile> hand5 = {r4, k5_tile, k6_tile}; // R4 (plays 1 to extend R-run), K5,K6 (plays 0)
    std::optional<Move> move5 = MoveFinder::find_best_move(cb5, hand5);
    assert(move5.has_value());
    if (move5) {
        assert(move5->tiles_played_count == 1);
        assert(sorted(move5->remaining_hand) == sorted({k5_tile,k6_tile}));
        assert(move5->new_board_state.getAllTiles().size() == 10);
        BoardState expected_b;
        expected_b.addSet(GameSet({r1,r2,r3,r4}, SetType::RUN));
        expected_b.addSet(GameSet({b1,b2,b3}, SetType::RUN));
        expected_b.addSet(GameSet({y1,y2,y3}, SetType::RUN));
        assert(areBoardStatesEquivalent(move5->new_board_state, expected_b, true));
    }
    std::cout << "find_best_move TC5 (Many board, few hand): Passed" << std::endl;

    // Test Case 6: Many tiles on board, many in hand - complex recombination
    BoardState cb6;
    cb6.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    cb6.addSet(GameSet({b5,b6,b7}, SetType::RUN));
    std::vector<Tile> hand6 = {r4, b4, y10,y11,y12}; // R4 (extends R), B4 (prepends to B), Y10,11,12 (new Y-run)
                                                   // Play all 5 tiles.
    std::optional<Move> move6 = MoveFinder::find_best_move(cb6, hand6);
    assert(move6.has_value());
    if (move6) {
        assert(move6->tiles_played_count == 5);
        assert(move6->remaining_hand.empty());
        assert(move6->new_board_state.getAllTiles().size() == 11);

        BoardState expected_b;
        expected_b.addSet(GameSet({r1,r2,r3,r4}, SetType::RUN));
        expected_b.addSet(GameSet({b4,b5,b6,b7}, SetType::RUN));
        expected_b.addSet(GameSet({y10,y11,y12}, SetType::RUN));
        assert(areBoardStatesEquivalent(move6->new_board_state, expected_b, true));
    }
    std::cout << "find_best_move TC6 (Many board, many hand - complex): Passed" << std::endl;
    std::cout << "--- MoveFinder::find_best_move ALL CASES PASSED ---" << std::endl;
}


int main() {
    std::cout << "Initial tile generation from utilities: " << generateAllTiles().size() << " tiles." << std::endl;

	testCoreGameLogicAndStructures();
	testSetFinderFunctionality();
    testBoardValidityChecks();
    testCanAddTilesToBoardFunctionality();
    testFindBestMove();

#ifdef ENABLE_PERFORMANCE_TRACING
    PerformanceTracer::print_performance_report();
#else
    std::cout << "\nPerformance tracing not enabled." << std::endl;
#endif

	return 0;
}
