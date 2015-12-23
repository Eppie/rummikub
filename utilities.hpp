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

