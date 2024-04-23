#ifndef COMPLETE_H
#define COMPLETE_H

#include <vector>
#include <iostream>
#include "inequality.h"
#include "constant_inequality.h"
#include "inequality_factory.h"
#include "unary_inequality.h"
#include "planar_comparator.h"

class complete{

    private:

    static std::ofstream detailed_stats;
    static int intermediate_result_call;
    static std::shared_ptr<inequality> in_result;

    static bool const debugging;
    
    public:

    static int modulo(int i, int l);
    
    static void closure(std::string in_dir, std::string out_dir);
    
    static std::vector<std::shared_ptr<inequality>> closure(std::vector<std::shared_ptr<inequality>> cs);

	static std::vector<std::shared_ptr<inequality>> insert(std::vector<std::shared_ptr<inequality>> cs);

	static std::vector<std::shared_ptr<inequality>> insert(std::shared_ptr<inequality> c, std::vector<std::shared_ptr<inequality>> cs, std::vector<std::string> vars);

    static std::vector<std::shared_ptr<inequality>> result(std::vector<std::shared_ptr<inequality>> cs1, std::vector<std::shared_ptr<inequality>> cs2);
    
 	static std::vector<std::shared_ptr<inequality>> result(std::vector<std::shared_ptr<inequality>> cs, std::shared_ptr<inequality> i2);

 	static std::vector<std::shared_ptr<inequality>> result(std::shared_ptr<inequality>  i1, std::shared_ptr<inequality>  i2);

    static int sign(mp_integer a);

    static std::vector<std::shared_ptr<inequality>> project(std::vector<std::shared_ptr<inequality>> cs, std::vector<std::string> vars);

    static std::vector<std::shared_ptr<inequality>> remove_constant_inequalities(std::vector<std::shared_ptr<inequality>> cs);

    static std::vector<std::shared_ptr<inequality>> remove_redundant_unary_inequalities(std::vector<std::shared_ptr<inequality>> cs);

    static std::vector<std::shared_ptr<inequality>> inc_complete(std::vector<std::shared_ptr<inequality>> cs, std::shared_ptr<inequality> c0);

};

extern void remove_all(std::vector<std::shared_ptr<inequality>> &cs, std::vector<std::shared_ptr<inequality>>&ds);

extern std::vector<std::shared_ptr<inequality>> make_bottom();

extern bool is_bottom(std::vector<std::shared_ptr<inequality>>);

#endif