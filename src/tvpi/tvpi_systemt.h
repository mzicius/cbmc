#ifndef TVPI_SYSTEMT_H
#define TVPI_SYSTEMT_H

#include <util/mp_arith.h>

#include "inequality.h"

class tvpi_systemt
{
public:

  tvpi_systemt();
  typedef mp_integer dimensiont;
  mp_integer dimension_counter;
  std::vector<std::shared_ptr<inequality>> constraints;

  void make_unsat_system();
  
  dimensiont add_new_dimension();

  //take dimension
  void existential_project(mp_integer dimensiont);

  std::vector<std::shared_ptr<inequality>> filter_ineqs(mp_integer dimensiont);

  //strings are d0 - dn where n is the dimentiont 
  void add_inequality(
    mp_integer a,
    std::string x,
    mp_integer b,
    std::string y,
    mp_integer c);
};

#endif