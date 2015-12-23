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
 *   Organization:  SevOne
 *
 * =====================================================================================
 */

#include "utilities.hpp"
#include <assert.h>

vector<Tile> allTiles = generateAllTiles();

int main() {
	assert( allTiles.size() == 104 );
	return 0;
}

