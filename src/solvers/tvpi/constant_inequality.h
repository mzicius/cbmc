#ifndef CONSTANT_INEQUALITY_H
#define CONSTANT_INEQUALITY_H

#include "inequality.h"

class constant_inequality: public inequality{

public:

    bool sat;

    constant_inequality(bool _sat);

    std::vector<std::string> vars() override;
    
    std::vector<coordinate> intercepts(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max) override;

    std::vector<coordinate> halfspace(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max) override;

    std::shared_ptr<inequality> widen(std::vector<std::string> vars) override;

    int compare_to(std::shared_ptr<inequality> inequality);

    std::string to_string() override;
    
    std::string to_ltx_label() override;

};

extern int bool_compare(const bool a, const bool b);

extern std::shared_ptr<constant_inequality> cast_to_const(std::shared_ptr<inequality> i);

#endif