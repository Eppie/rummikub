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
 *   Organization:  SevOne
 *
 * =====================================================================================
 */

#include "Tile.hpp"
#include <vector>
#include <algorithm>

using namespace std;

auto isBlue = [](Tile t){ return t.getColor() == color::blue; };
auto isPurple = [](Tile t){ return t.getColor() == color::purple; };
auto isRed = [](Tile t){ return t.getColor() == color::red; };
auto isYellow = [](Tile t){ return t.getColor() == color::yellow; };

template<typename Pred, typename T>
vector<T> keepIf( Pred predicate, const vector<T>& xs ) {
	vector<T> result;
	copy_if( begin( xs ), end( xs ), back_inserter( result ), predicate );
	return result;
}

template<typename T>
vector<T> onlyBlues( const vector<T>& tiles ) {
	return keepIf( isBlue, tiles );
}

template<typename T>
vector<T> onlyPurples( const vector<T>& tiles ) {
	return keepIf( isPurple, tiles );
}

template<typename T>
vector<T> onlyReds( const vector<T>& tiles ) {
	return keepIf( isRed, tiles );
}

template<typename T>
vector<T> onlyYellows( const vector<T>& tiles ) {
	return keepIf( isYellow, tiles );
}

vector<vector<Tile>> findGroups( vector<Tile> tiles ) {
	vector<vector<Tile>> result;
	return result;
}

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

