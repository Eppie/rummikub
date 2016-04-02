/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  Main file for Rummikub game
 *
 *        Version:  1.0
 *        Created:  08/17/2015 04:39:42 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Andrew Epstein
 *   Organization:  SevOne
 *
 * =====================================================================================
 */

#include "Tile.cpp"
#include "utilities.hpp"
#include <algorithm>
#include <vector>

int main() {
	srand( time( nullptr ) );
	vector<Tile> allTiles = generateAllTiles();
	shuffle( &allTiles );
	vector<Tile> myHand = drawHand( &allTiles );
	cout << "My hand:" << endl;

	auto runs = findRuns( myHand );
	for( auto run : runs ) {
		cout << isValidRun( run ) << ": ";
		for( auto tile : run ) {
			tile.print();
		}
		cout << endl;
	}

	return 0;
}
