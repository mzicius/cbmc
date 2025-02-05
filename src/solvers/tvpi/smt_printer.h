#ifndef SMT_PRINTER_H
#define SMT_PRINTER_H
#include <string>
#include <util/mp_arith.h>
#include "inequality.h"

class smt_printer{

public:
static void print_smt_entailment();
static void print_smt_closure(std::string in_file_one, std::string in_file_two, std::string smt_out);
static void print_system(std::string in_dir,std::string out_dir);
static void print_hull(std::string in_one,std::string in_two, std::string in_three, std::string smt_out);
static std::string label(std::string s, mp_integer i);
static std::string sign_label(mp_integer m);
static mp_integer  abs_number(mp_integer m);

};

#endif
