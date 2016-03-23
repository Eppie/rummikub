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
#include <vector>
#include <algorithm>

template<typename T>
void shuffle( T iterable ) {
	random_shuffle( iterable->begin(), iterable->end() );
}

int main() {
	srand( time( NULL ) );
	vector<Tile> allTiles = generateAllTiles();
	shuffle( &allTiles );
	vector<Tile> myHand = drawHand( &allTiles );
	cout << "My hand:" << endl;

	auto runs = findRuns( myHand );
	for( auto run : runs ) {
		for( auto tile : run ) {
			tile.print();
		}
		cout << endl;
	}

	return 0;
}
