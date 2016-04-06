/*
 * =====================================================================================
 *
 *       Filename:  groups.hpp
 *
 *    Description:  Functions relating to groups
 *
 *        Version:  1.0
 *        Created:  4/03/2016 02:36:16 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Andrew Epstein
 *
 * =====================================================================================
 */

#pragma once
#include "utilities.hpp"

/*
 * TODO: implement this function
 * @param vector<Tile> tiles
 * @return vector<vector<Tile>>
 */
vector<vector<Tile>> findGroups( vector<Tile> tiles ) {
	vector<vector<Tile>> result;
	unordered_map<int, vector<Tile>> mymap;

	for( auto tile : tiles ) {
		mymap[tile.getNumber()].push_back( tile );
	}

	for( auto group : mymap ) {
		result.push_back( group.second );
	}

	// At this point, we've separated the tiles by number
	return result;
}

bool isValidGroup( vector<Tile> tiles ) {
	// Groups must be either 3 or 4 tiles
	if( tiles.size() < 3 || tiles.size() > 4 ) {
		return false;
	}

	set<int> uniqueColors;
	auto number = tiles[0].getNumber();

	for( auto tile : tiles ) {
		uniqueColors.insert( tile.getColor() );

		// All tiles must have the same number
		if( tile.getNumber() != number ) {
			return false;
		}
	}

	// All tiles must have a different color
	if( uniqueColors.size() != tiles.size() ) {
		return false;
	}

	return true;
}

