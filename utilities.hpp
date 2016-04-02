/*
 * =====================================================================================
 *
 *       Filename:  utilities.hpp
 *
 *    Description:  Some extra helper functions
 *
 *        Version:  1.0
 *        Created:  12/22/2015 11:45:06 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Andrew Epstein
 *
 * =====================================================================================
 */

#include "Tile.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>

using namespace std;

/*
 * Returns true if predicate evaluates to true for all elements in container
 * Example: all( isPrime, { 3, 5, 6 } ) == false
 *          all( isPrime, { 3, 5, 7 } ) == true
 * @param Predicate p
 * @param Container& xs
 * @return bool
 */
template <typename Predicate, typename Container>
bool all( Predicate p, const Container &xs ) {
	return all_of( begin( xs ), end( xs ), p );
}

// TODO: Find a way to not have to manually specify each of these functions
auto isBlue = []( Tile t ) {
	return t.getColor() == color::blue;
};

auto isPurple = []( Tile t ) {
	return t.getColor() == color::purple;
};

auto isRed = []( Tile t ) {
	return t.getColor() == color::red;
};

auto isYellow = []( Tile t ) {
	return t.getColor() == color::yellow;
};

template<typename T>
void shuffle( T iterable ) {
	random_shuffle( iterable->begin(), iterable->end() );
}

/*
 * @param Pred predicate
 * @param const vector<T>&
 * @return vector<T>
 */
template<typename Pred, typename T>
vector<T> keepIf( Pred predicate, const vector<T> &xs ) {
	vector<T> result;
	copy_if( begin( xs ), end( xs ), back_inserter( result ), predicate );
	return result;
}

// TODO: Find a way to not have to manually specify each of these functions
template<typename T>
vector<T> onlyBlues( const vector<T> &tiles ) {
	return keepIf( isBlue, tiles );
}

template<typename T>
vector<T> onlyPurples( const vector<T> &tiles ) {
	return keepIf( isPurple, tiles );
}

template<typename T>
vector<T> onlyReds( const vector<T> &tiles ) {
	return keepIf( isRed, tiles );
}

template<typename T>
vector<T> onlyYellows( const vector<T> &tiles ) {
	return keepIf( isYellow, tiles );
}

/*
 * TODO: implement this function
 * @param vector<Tile> tiles
 * @return vector<vector<Tile>>
 */
vector<vector<Tile>> findGroups( vector<Tile> tiles ) {
	vector<vector<Tile>> result;
	unordered_map<int, vector<Tile>> mymap;
	for( auto tile : tiles ) {
		mymap[tile.getColor()].push_back( tile );
	}
	for( auto group : mymap ) {
		result.push_back( group.second );
	}
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

/*
 * TODO: Fix this bug: ( [ 1, 1, 2, 2, 3, 3, 4, 4 ] --> [ [ 1 ], [ 1, 2 ], [ 2, 3 ], [ 3, 4 ], [ 4 ] ] )
 * Precondition: The input vector MUST be sorted.
 * @param vector<Tile> tiles
 * @return vector<vector<Tile>>
 */
vector<vector<Tile>> findRuns( vector<Tile> tiles ) {
	vector<vector<Tile>> result( 1, vector<Tile>( 1, tiles[0] ) );

	for_each( tiles.begin() + 1, tiles.end(), [&]( Tile i ) {
		if( result.back().back().getNumber() == i.getNumber() - 1 ) {
			result.back().push_back( i );
		} else {
			result.push_back( vector<Tile>( 1, i ) );
		}
	} );

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

/*
 * @return vector<Tile>
 */
vector<Tile> generateAllTiles() {
	vector<Tile> result;

	for( int i = 1; i <= 4; i++ ) {
		for( int j = 1; j <= 13; j++ ) {
			Tile t1 = Tile( j, i );
			result.push_back( t1 );
			Tile t2 = Tile( j, i );
			result.push_back( t2 );
		}
	}

	return result;
}

/*
 * Precondition: The input vector must be in a randomized order.
 * Removes 14 tiles from the input vector and returns a new vector with those 14 tiles.
 * @param vector<Tile>* allTiles
 * @return vector<Tile>
 */
vector<Tile> drawHand( vector<Tile>* allTiles ) {
	vector<Tile> result;

	for( int i = 0; i < 14; i++ ) {
		result.push_back( allTiles->back() );
		allTiles->pop_back();
	}

	sort( result.begin(), result.end() );
	return result;
}

