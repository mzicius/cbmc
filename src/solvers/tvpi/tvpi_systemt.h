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

  const static bool dbg_all = false;
  tvpi_systemt();
  typedef mp_integer dimensiont;
  std::vector<std::shared_ptr<inequality>> constraints;
  dimensiont get_current_dim() const;

  void make_unsat_system();

  dimensiont add_new_dimension();

  //project out a dimension from the current system
  void existential_project(mp_integer dimensiont);

  //strings are d0 - dn where n is the dimentiont
  void add_inequality(
    mp_integer a,
    std::string x,
    mp_integer b,
    std::string y,
    mp_integer c);

  void add_inequality(
    rationalt a,
    std::string x,
    rationalt b,
    std::string y,
    rationalt c);

  void print_system();

  std::vector<std::shared_ptr<inequality>>
  filter(const std::vector<std::string> &target_vars) const;

  std::optional<rationalt> get_ub(mp_integer dimensiont);

  std::optional<rationalt> get_lb(mp_integer dimensiont);

private:
  static dimensiont dim_counter;
};

#endif