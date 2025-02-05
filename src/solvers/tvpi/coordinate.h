#ifndef COORDINATE_H
#define COORDINATE_H
#include <string>

class coordinate {
    
    public:
    
    double x, y;

    coordinate(double _x, double _y);

    std::string to_string();

};
    extern bool coord_compare(const coordinate &c1, const coordinate &c2);

#endif
