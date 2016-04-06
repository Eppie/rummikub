/*
 * =====================================================================================
 *
 *       Filename:  runs.hpp
 *
 *    Description:  Functions relating to runs
 *
 *        Version:  1.0
 *        Created:  4/03/2016 02:47:57 PM
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
 * @param vector<Tile> tiles
 * @return vector<vector<Tile>>
 */
vector<vector<Tile>> findRuns( vector<Tile> tiles ) {
	sort( tiles.rbegin(), tiles.rend() );
	vector<vector<Tile>> result;

	for( auto && tile : reverse( tiles ) ) {
		bool added = false;

		for( auto && run : result ) {
			if( run.back().getNumber() == tile.getNumber() - 1 && run.back().getColor() == tile.getColor() ) {
				run.push_back( tile );
				added = true;
				break;
			}
		}

		if( !added ) {
			result.push_back( vector<Tile>( 1, tile ) );
		}
	}

	return result;

}

bool isValidRun( vector<Tile> tiles ) {
	// Runs must be at least 3 tiles long
	if( tiles.size() < 3 ) {
		return false;
	}

	sort( tiles.begin(), tiles.end() );

	auto color = tiles[0].getColor();
	auto number = tiles[0].getNumber();

	for( auto tile : tiles ) {
		// Runs must consist of tiles that are all the same color
		if( tile.getColor() != color ) {
			return false;
		}

		// Runs must be monotonically increasing by 1
		if( tile.getNumber() != number ) {
			return false;
		}

		number++;
	}

	return true;
}

