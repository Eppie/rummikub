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

vector<Tile> drawHand( vector<Tile>* allTiles ) {
	vector<Tile> result;

	for( int i = 0; i < 14; i++ ) {
		result.push_back( allTiles->back() );
		allTiles->pop_back();
	}

	sort( result.begin(), result.end() );
	return result;
}

int main() {
	srand( time( NULL ) );
	vector<Tile> allTiles = generateAllTiles();
	shuffle( &allTiles );
	vector<Tile> myHand = drawHand( &allTiles );
	cout << "My hand:" << endl;

	for( auto t : myHand ) {
		t.print();
	}

	cout << "Remaining blues:" << endl;
	sort( allTiles.begin(), allTiles.end() );
	auto blues = onlyBlues( allTiles );

	for( auto t : blues ) {
		t.print();
	}

	return 0;
}
