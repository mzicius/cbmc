#ifndef UNARY_INEQUALITY_H
#define UNARY_INEQUALITY_H

#include "inequality.h"
#include "dyadic_inequality.h"

class unary_inequality: public inequality{

public:

std::string x;
mp_integer a, c;

unary_inequality(std::string _x, mp_integer _a, mp_integer _c);

std::vector<std::string> vars() override;

int sign_a();

std::vector<coordinate> intercepts(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max) override;

std::vector<coordinate> halfspace(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max) override;

std::shared_ptr<inequality> widen(std::vector<std::string> vars) override;

std::string to_string() override;

std::string to_ltx_label() override;

};

extern std::shared_ptr<unary_inequality> cast_to_unary(std::shared_ptr<inequality> i);

#endif