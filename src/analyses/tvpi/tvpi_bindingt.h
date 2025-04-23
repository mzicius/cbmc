#ifndef TVPI_BINDINGT_H
#define TVPI_BINDINGT_H

#include <util/mp_arith.h>

#include <solvers/tvpi/tvpi_systemt.h>

#include <vector>

#include "../ai_domain.h"
#include "../variable-sensitivity/abstract_environment.h"

class tvpi_bindingt
{
public:
  typedef std::map<tvpi_systemt::dimensiont, mp_integer> reference_map;
  typedef std::map<symbol_exprt, tvpi_systemt::dimensiont> binding_map;
  reference_map references;
  binding_map binding;

  void set_binding(symbol_exprt symbol, tvpi_systemt::dimensiont dim);
  void print_binding() const;
  void print_references() const;
  void add_tmp_ref(tvpi_systemt::dimensiont dim);
  tvpi_systemt::dimensiont lookup_binding(symbol_exprt symbol);
  bool wipe_binding(tvpi_systemt &sys);

  binding_map copy_map() const;
  void override_binding(const binding_map &new_binding);

};

#endif