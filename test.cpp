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
#include "MoveFinder.hpp"
#include "GameTypes.hpp"

#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <optional>

// Existing global variable
vector<Tile> allTiles = generateAllTiles();

// Helper to sort a vector of tiles
std::vector<Tile> sorted(std::vector<Tile> tiles) {
    std::sort(tiles.begin(), tiles.end());
    return tiles;
}

// Updated Helper to compare vectors of GameSet for testing purposes
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

// Updated Helper to compare BoardState objects
bool areBoardStatesEquivalent(BoardState bs1, BoardState bs2, bool verbose = false) {
    return areGameSetVectorsEqual("BoardStateComparison", bs1.sets, bs2.sets, verbose);
}


void testBoardStructures() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing Board Structures ---" << std::endl;

    Tile r1(1, red), r2(2, red), r3(3, red); // Tiles for the run
    Tile r4_g(4, red), b4_g(4, blue), y4_g(4, yellow); // Tiles for a distinct group
    Tile r1_invalid(1,red), r3_invalid(3,red), r4_invalid(4,red); // For invalid run test
    Tile r3_dup_color_test(3,red), b3_dup_color_test(3,blue); // For invalid group test


    GameSet run_set({r1, r2, r3}, SetType::RUN);
    std::cout << "Testing Run: "; run_set.print();
    assert(run_set.isValid() && run_set.type == SetType::RUN && run_set.tiles.size() == 3);

    GameSet group_set({r4_g, b4_g, y4_g}, SetType::GROUP);
    std::cout << "Testing Group: "; group_set.print();
    assert(group_set.isValid() && group_set.type == SetType::GROUP);

    GameSet invalid_run_set({r1_invalid, r3_invalid, r4_invalid}, SetType::RUN); //r1,r3,r4
    std::cout << "Testing Invalid Run (gap): "; invalid_run_set.print();
    assert(!invalid_run_set.isValid());

    // Invalid group due to duplicate color (Red 3, Red 3, Blue 3)
    GameSet invalid_group_set_dup_color({r3_dup_color_test, Tile(3,red) , b3_dup_color_test}, SetType::GROUP);
    std::cout << "Testing Invalid Group (duplicate color): "; invalid_group_set_dup_color.print();
    assert(!invalid_group_set_dup_color.isValid());

    BoardState board;
    std::cout << "\nInitial Board State:" << std::endl; board.print();
    board.addSet(run_set);
    board.addSet(group_set);
    std::cout << "\nBoard State after adding sets:" << std::endl; board.print();
    assert(board.isValidBoard()); // Now this should pass as tiles R1,R2,R3 and R4g,B4g,Y4g are distinct

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
    assert(board == board_copy);

    std::cout << "--- Board Structures Tests Passed ---" << std::endl;
}

void testSetFinder() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing SetFinder ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red), r5(5,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue), b4_b(4,blue); // Renamed b4 to b4_b
    Tile y3(3,yellow), y5(5,yellow);
    Tile p3(3,purple), p7_p(7,purple); // Renamed p7 to p7_p

    std::vector<Tile> tc1_tiles = {r1, r2, r3};
    std::vector<GameSet> tc1_expected = {GameSet({r1,r2,r3}, SetType::RUN)};
    assert(areGameSetVectorsEqual("TC1 Basic Run", SetFinder::find_all_possible_sets(tc1_tiles), tc1_expected, true));

    std::vector<Tile> tc2_tiles = {b1, b2, b3, b4_b};
    std::vector<GameSet> tc2_expected = {
        GameSet({b1,b2,b3}, SetType::RUN), GameSet({b1,b2,b3,b4_b}, SetType::RUN), GameSet({b2,b3,b4_b}, SetType::RUN)};
    assert(areGameSetVectorsEqual("TC2 Multiple Runs", SetFinder::find_all_possible_sets(tc2_tiles), tc2_expected, true));

    std::vector<Tile> tc3_tiles = {r3, b3, y3};
    std::vector<GameSet> tc3_expected = {GameSet({b3,r3,y3}, SetType::GROUP)};
    assert(areGameSetVectorsEqual("TC3 Basic Group", SetFinder::find_all_possible_sets(tc3_tiles), tc3_expected, true));

    Tile r7(7,red), b7(7,blue), y7(7,yellow);
    std::vector<Tile> tc4_tiles = {r7,b7,y7,p7_p};
    std::vector<GameSet> tc4_expected = {
        GameSet({b7,p7_p,r7},SetType::GROUP), GameSet({b7,p7_p,y7},SetType::GROUP),
        GameSet({b7,r7,y7},SetType::GROUP), GameSet({p7_p,r7,y7},SetType::GROUP),
        GameSet({b7,p7_p,r7,y7},SetType::GROUP)};
    assert(areGameSetVectorsEqual("TC4 Multiple Groups", SetFinder::find_all_possible_sets(tc4_tiles), tc4_expected, true));

    std::vector<Tile> tc5_tiles = {r1,r2,r3,b3,y3};
    std::vector<GameSet> tc5_expected = { GameSet({r1,r2,r3},SetType::RUN), GameSet({b3,r3,y3},SetType::GROUP)};
    assert(areGameSetVectorsEqual("TC5 Mixed Run/Group", SetFinder::find_all_possible_sets(tc5_tiles), tc5_expected, true));

    std::cout << "--- SetFinder Tests Passed ---" << std::endl;
}

void testBoardValidity() { // Combined isValidBoard and is_board_valid tests
    TRACE_FUNCTION();
    std::cout << "\n--- Testing Board Validity (BoardState::isValidBoard & is_board_valid) ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red), r4_r(4,red); // Renamed r4 to r4_r
    Tile b4(4,blue), y4(4,yellow);

    GameSet valid_run({r1,r2,r3}, SetType::RUN);
    GameSet valid_group({r4_r,b4,y4}, SetType::GROUP);
    GameSet group_with_r1({r1, Tile(1,blue), Tile(1,yellow)}, SetType::GROUP); // R1, B1, Y1

    BoardState b_empty; assert(b_empty.isValidBoard() && is_board_valid(b_empty.sets));
    BoardState b_valid_single; b_valid_single.addSet(valid_run);
    assert(b_valid_single.isValidBoard() && is_board_valid(b_valid_single.sets));
    BoardState b_valid_multi; b_valid_multi.addSet(valid_run); b_valid_multi.addSet(valid_group);
    assert(b_valid_multi.isValidBoard() && is_board_valid(b_valid_multi.sets));

    std::vector<GameSet> sets_for_invalid_board = {valid_run, group_with_r1}; // R1 duplicated
    BoardState b_invalid_dup(sets_for_invalid_board);
    assert(!b_invalid_dup.isValidBoard() && !is_board_valid(b_invalid_dup.sets));
    std::cout << "--- Board Validity Tests Passed ---" << std::endl;
}

void testCanAddTilesToBoardFunctionality() { // Renamed from testCanAddTilesToBoard
    TRACE_FUNCTION();
    std::cout << "\n--- Testing BoardManipulation::can_add_tiles_to_board ---" << std::endl;
    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue), b4_b(4,blue); // Renamed b4 to b4_b
    Tile y4(4,yellow), p4(4,purple);

    BoardState empty_board;
    std::vector<Tile> tiles_add1 = {r1,r2,r3};
    std::optional<BoardState> res1 = BoardManipulation::can_add_tiles_to_board(empty_board, tiles_add1);
    assert(res1.has_value());
    if(res1){ BoardState exp1; exp1.addSet(GameSet(tiles_add1,SetType::RUN)); assert(areBoardStatesEquivalent(*res1,exp1));}

    std::vector<Tile> tiles_add2 = {r1,r2}; // Cannot form set
    std::optional<BoardState> res2 = BoardManipulation::can_add_tiles_to_board(empty_board, tiles_add2);
    assert(!res2.has_value());

    BoardState board3; board3.addSet(GameSet({b1,b2,b3},SetType::RUN));
    std::vector<Tile> tiles_add3 = {r1,r2,r3};
    std::optional<BoardState> res3 = BoardManipulation::can_add_tiles_to_board(board3, tiles_add3);
    assert(res3.has_value());
    if(res3){ BoardState exp3; exp3.addSet(GameSet({b1,b2,b3},SetType::RUN)); exp3.addSet(GameSet({r1,r2,r3},SetType::RUN)); assert(areBoardStatesEquivalent(*res3,exp3));}

    // Test Case: Add tile that extends an existing run (via rebuild)
    BoardState board5; board5.addSet(GameSet({r1,r2,r3}, SetType::RUN));
    std::vector<Tile> tiles_add5 = {r4};
    std::optional<BoardState> res5 = BoardManipulation::can_add_tiles_to_board(board5, tiles_add5);
    assert(res5.has_value());
    if(res5) { BoardState exp5; exp5.addSet(GameSet({r1,r2,r3,r4},SetType::RUN)); assert(areBoardStatesEquivalent(*res5,exp5,true));}

    // Test Case: Add tiles requiring breaking and reforming
    BoardState board6; board6.addSet(GameSet({r1,r2,r3,r4}, SetType::RUN)); board6.addSet(GameSet({Tile(5,yellow),Tile(6,yellow),Tile(7,yellow)}, SetType::RUN));
    std::vector<Tile> tiles_add6 = {b4_b, p4}; // Add B4, P4 - to use R4 from board
    std::optional<BoardState> res6 = BoardManipulation::can_add_tiles_to_board(board6, tiles_add6);
    assert(res6.has_value());
    if(res6){
        BoardState exp6;
        exp6.addSet(GameSet({r1,r2,r3},SetType::RUN));
        exp6.addSet(GameSet({r4,b4_b,p4},SetType::GROUP));
        exp6.addSet(GameSet({Tile(5,yellow),Tile(6,yellow),Tile(7,yellow)},SetType::RUN));
        assert(areBoardStatesEquivalent(*res6,exp6,true));
    }
    std::cout << "--- can_add_tiles_to_board Tests Passed ---" << std::endl;
}

void testFindBestMove() {
    TRACE_FUNCTION();
    std::cout << "\n--- Testing MoveFinder::find_best_move ---" << std::endl;

    Tile r1(1,red), r2(2,red), r3(3,red), r4(4,red), r5(5,red);
    Tile b1(1,blue), b2(2,blue), b3(3,blue), b4_b(4,blue), b5_b(5,blue), b6_b(6,blue), b7_b(7,blue), b8_b(8,blue); // Renamed blue tiles
    Tile y1(1,yellow), y2(2,yellow), y3(3,yellow);
    Tile y10(10,yellow), y11(11,yellow), y12(12,yellow);
    Tile k5_tile(5, 0);
    Tile k6_tile(6,0);

    // TC1: Simple play all
    BoardState cb1; std::vector<Tile> hand1 = {r1, r2, r3};
    std::optional<Move> move1 = MoveFinder::find_best_move(cb1, hand1);
    assert(move1.has_value() && move1->tiles_played_count == 3 && move1->remaining_hand.empty());
    if(move1){BoardState exp; exp.addSet(GameSet({r1,r2,r3},SetType::RUN)); assert(areBoardStatesEquivalent(move1->new_board_state,exp));}
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
    BoardState cb5; cb5.addSet(GameSet({r1,r2,r3},SetType::RUN)); cb5.addSet(GameSet({b1,b2,b3},SetType::RUN)); cb5.addSet(GameSet({y1,y2,y3},SetType::RUN));
    std::vector<Tile> hand5 = {r4, k5_tile, k6_tile};
    std::optional<Move> move5 = MoveFinder::find_best_move(cb5, hand5);
    assert(move5.has_value() && move5->tiles_played_count == 1 && sorted(move5->remaining_hand) == sorted({k5_tile,k6_tile}));
    if(move5){BoardState exp; exp.addSet(GameSet({r1,r2,r3,r4},SetType::RUN)); exp.addSet(GameSet({b1,b2,b3},SetType::RUN)); exp.addSet(GameSet({y1,y2,y3},SetType::RUN)); assert(areBoardStatesEquivalent(move5->new_board_state,exp,true));}
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

int main() {
    assert( allTiles.size() == 104 ); // From global `allTiles`
    std::cout << "Initial tile generation global check passed." << std::endl;

	testBoardStructures();
	testSetFinder();
    testBoardValidity();
    testCanAddTilesToBoardFunctionality(); // Renamed
    testFindBestMove();

#ifdef ENABLE_PERFORMANCE_TRACING
    PerformanceTracer::print_performance_report();
#else
    std::cout << "\nPerformance tracing not enabled (define TRACE=1 with make)." << std::endl;
#endif

	return 0;
}
