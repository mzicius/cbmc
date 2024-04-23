#include "vertex.h"

vertex::vertex(rationalt _x, rationalt _y):
x(_x),
y(_y)
{}

void vertex::print(){

   std::cout<<"("<<x<<","<<y<<")"<<std::endl;
}
