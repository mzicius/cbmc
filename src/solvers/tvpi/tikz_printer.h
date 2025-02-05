#ifndef TIKZ_PRINTER_H
#define TIKZ_PRINTER_H

#include "inequality.h"


class tikz_printer{

public:
//static void print_tikz(std::vector<std::shared_ptr<inequality>> cs,std::string file_name);
//static void print_tikz_hull(std::vector<std::shared_ptr<inequality>> cs,std::vector<std::shared_ptr<inequality>> cs2,std::vector<std::shared_ptr<inequality>> out, std::string file_name);
static void print_tikz_system(std::vector<std::shared_ptr<inequality>> cs, std::string file_name, std::string color, int half_range);
static void print_grid(std::ofstream& figure,mp_integer max_range,std::set<std::string> vars);
static mp_integer update_range(mp_integer max_range,std::vector<std::shared_ptr<inequality>> cs);
static std::set<std::string> get_vars(std::vector<std::shared_ptr<inequality>> cs);
    
};

#endif
