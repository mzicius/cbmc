#ifndef TVPI_SYSTEMT_H
#define TVPI_SYSTEMT_H

#include <util/mp_arith.h>

#include "complete.h"
#include "inequality.h"
#include "unary_inequality.h"

#include <map>

class tvpi_systemt
{
public:
  tvpi_systemt();
  typedef mp_integer dimensiont;
  mp_integer dimension_counter;
  std::vector<std::shared_ptr<inequality>> constraints;
  std::map<dimensiont, mp_integer> references;

  void make_unsat_system();

  dimensiont add_new_dimension();

  //project out a dimension from the current system
  void existential_project(mp_integer dimensiont);

  std::vector<std::shared_ptr<inequality>> filter_ineqs(mp_integer dimensiont);

  //strings are d0 - dn where n is the dimentiont
  void add_inequality(
    mp_integer a,
    std::string x,
    mp_integer b,
    std::string y,
    mp_integer c);

  void print_system();

  std::vector<std::string> extract_vars();

  std::optional<mp_integer> get_ub(mp_integer dimensiont);

  std::optional<mp_integer> get_lb(mp_integer dimensiont);
  
};

#endif