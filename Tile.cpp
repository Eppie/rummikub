/*
 * =====================================================================================
 *
 *       Filename:  Tile.cpp
 *
 *    Description:  Tile class for game of Rummikub
 *
 *        Version:  1.0
 *        Created:  08/16/2015 09:32:25 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Andrew Epstein
 *
 * =====================================================================================
 */

#include "Tile.hpp"

void Tile::print() const {
	if( color == 1 ) {
		cout << BLUE;
	} else if( color == 2 ) {
		cout << PURPLE;
	} else if( color == 3 ) {
		cout << RED;
	} else if( color == 4 ) {
		cout << YELLOW;
	}

	cout << number << " " << NONE;
}

bool Tile::operator==( const Tile &other ) const {
	if( this->getColor() != other.getColor() ) {
		return false;
	} else if( this->getNumber() != other.getNumber() ) {
		return false;
	}

	return true;
}

bool operator<( const Tile &t1, const Tile &t2 ) {
	if( t1.getColor() != t2.getColor() ) {
		return t1.getColor() < t2.getColor();
	} else {
		return t1.getNumber() < t2.getNumber();
	}
}
