/*
 * =====================================================================================
 *
 *       Filename:  Tile.hpp
 *
 *    Description:  Tile class for game of Rummikub
 *
 *        Version:  1.0
 *        Created:  08/16/2015 09:32:18 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  Andrew Epstein
 *
 * =====================================================================================
 */

#pragma once
#include <iostream>
#define COLOR 1
#include "color.h"

using namespace std;

enum color {
	blue = 1,
	purple = 2,
	red = 3,
	yellow = 4
};

class Tile {
	int number;
	int color;
public:
	void setNumber( int n ) {
		number = n;
	}
	void setColor( int c ) {
		color = c;
	}
	int getNumber() const {
		return number;
	}
	int getColor() const {
		return color;
	}
	void print();
	bool operator==( const Tile &other );
	Tile( int n, int c ) {
		number = n;
		color = c;
	}
};

bool operator<( const Tile &t1, const Tile &t2 ) {
	if( t1.getColor() != t2.getColor() ) {
		return t1.getColor() < t2.getColor();
	} else {
		return t1.getNumber() < t2.getNumber();
	}
}

