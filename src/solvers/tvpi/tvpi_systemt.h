#ifndef TVPI_SYSTEMT_H
#define TVPI_SYSTEMT_H

#include <util/mp_arith.h>
#include "complete.h"
#include "inequality.h"
#include "unary_inequality.h"

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
  std::vector<std::shared_ptr<inequality>> filter_unary(std::vector<std::string> vars);
  std::vector<std::shared_ptr<inequality>> filter_dyadic(std::vector<std::string> vars); 

  //strings are d0 - dn where n is the dimentiont
  void add_inequality(
    mp_integer a,
    std::string x,
    mp_integer b,
    std::string y,
    mp_integer c);

  std::optional<mp_integer> get_ub(mp_integer dimensiont);
  std::optional<mp_integer> get_lb(mp_integer dimensiont);
  std::vector<std::shared_ptr<inequality>> relabel(mp_integer old_d, mp_integer new_d);

};

#endif