#include "coordinate.h"

coordinate::coordinate(double _x, double _y)
: x(_x)
, y(_y)
{}

std::string coordinate::to_string(){

    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";

}

bool coord_compare(const coordinate &c1, const coordinate &c2){
	
    if (c1.x != c2.x) return c1.x < c2.x;
  	else return c1.y < c2.y;

}
