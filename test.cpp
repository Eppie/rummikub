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
#include <cassert>

vector<Tile> allTiles = generateAllTiles();

int main() {
	assert( allTiles.size() == 104 );
	return 0;
}

