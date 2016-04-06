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

#pragma once
#include "Tile.hpp"
#include <vector>
#include <algorithm>
#include <set>
#include <unordered_map>

using namespace std;

template <typename T>
struct reversion_wrapper {
	T &iterable;
};

template <typename T>
auto begin( reversion_wrapper<T> w ) {
	return rbegin( w.iterable );
}

template <typename T>
auto end( reversion_wrapper<T> w ) {
	return rend( w.iterable );
}

template <typename T>
reversion_wrapper<T> reverse( T &&iterable ) {
	return { iterable };
}

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

