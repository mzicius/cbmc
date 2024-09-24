#ifndef TVPI_SYSTEMT_H
#define TVPI_SYSTEMT_H

#include <util/mp_arith.h>

#include "inequality.h"

class tvpi_systemt
{
public:
  std::vector<std::shared_ptr<inequality>> constraints;
  tvpi_systemt();
  void make_unsat_system();
  typedef mp_integer dimensiont;
  void existential_project(std::string var);
  dimensiont add_new_dimension();
  std::shared_ptr<inequality> add_inequality(
    mp_integer a,
    std::string x,
    mp_integer b,
    std::string y,
    mp_integer c);
};

#endif