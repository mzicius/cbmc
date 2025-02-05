#include <stdexcept>
#include "constant_inequality.h"

constant_inequality::constant_inequality(bool _sat)
:sat(_sat)
{}

std::vector<std::string> constant_inequality::vars(){
   return {};
}

std::vector<coordinate> constant_inequality::intercepts(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max){
    throw std::runtime_error("The vector of coordinates can't be instantiated.");
}

std::vector<coordinate> constant_inequality::halfspace(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max){
    throw std::runtime_error("The vector of coordinates can't be instantiated.");
}

std::shared_ptr<inequality> constant_inequality::widen(std::vector<std::string> vars){
    return nullptr;
}

int constant_inequality::compare_to(std::shared_ptr<inequality> inequality){

    if (inequality->arity() == 0)
	{
	   std::shared_ptr<constant_inequality> constant = std::dynamic_pointer_cast<constant_inequality>(inequality);
	   return bool_compare(sat, constant->sat);
       
	}
	else return -1;

}

std::string constant_inequality::to_string(){
   return bool_to_string(sat);
}

std::string constant_inequality::to_ltx_label(){
    return bool_to_string(sat);
}

int bool_compare(const bool a, const bool b){

    if(a==b) return 0;
    else if (a) return 1;
    else return -1;
}

extern std::shared_ptr<constant_inequality> cast_to_const(std::shared_ptr<inequality> i){
    return std::dynamic_pointer_cast<constant_inequality>(i);
}