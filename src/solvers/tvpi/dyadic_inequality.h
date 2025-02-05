#ifndef DYADIC_INEQUALITY_H
#define DYADIC_INEQUALITY_H
#include <cmath>
#include "coordinate.h"
#include "inequality.h"

class dyadic_inequality: public inequality{

public:

std::string x, y;

mp_integer a, b, c;

dyadic_inequality(std::string _x, std::string _y, mp_integer _a, mp_integer _b, mp_integer _c);

std::vector<std::string> vars() override;

int sign_a();

int sign_b();

std::vector<coordinate> intercepts(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max) override;

std::vector<coordinate> halfspace(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max) override;

std::shared_ptr<inequality> widen(std::vector<std::string> vars) override;

std::string to_string() override;

std::string to_ltx_label() override;

};

extern std::shared_ptr<dyadic_inequality> cast_to_dyadic(std::shared_ptr<inequality> i);

#endif