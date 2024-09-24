#ifndef TVPI_DOMAINT_H
#define TVPI_DOMAINT_H

#include "complete.h"
#include "tvpi_systemt.h"

#include <vector>

#include "../analyses/ai_domain.h"

class tvpi_domaint : public ai_domain_baset
{
public:
  tvpi_domaint();

  //static int result_call;
  //static std::chrono::milliseconds total_duration;
  //static std::size_t input_sys_size;
  //static std::size_t output_sys_size;

  virtual ~tvpi_domaint();

  /// Transform updates the domain with the effect of the instruction
  /// given by "from".
  /// As GOTO has multiple next instructions, the abstract trace "to"
  /// is passed so you can tell whether it is the branch taken or not.
  virtual void transform(
    const irep_idt &function_from,
    trace_ptrt from,
    const irep_idt &function_to,
    trace_ptrt to,
    ai_baset &ai,
    const namespacet &ns) override;

  /// Merges two domains together
  /// \return true if and only if *this has been modified / extended
  /// If it is true then it will requeue this domain for analysis
  bool merge(const tvpi_domaint &b, trace_ptrt, trace_ptrt);

  /// Set the domain to be empty, i.e. representing nothing
  void make_bottom() override;

  /// Set the domain to allow all possibilities
  void make_top() override;

  /// Set up the domain for the start of the program
  /// Part of ai_domain_baset
  /// Invoked by Abstract Interpretation Module (ai.cpp)
  void make_entry() override;

  /// Is the domain bottom or not
  bool is_bottom() const override;

  /// Is the domain top or not
  bool is_top() const override;

  /// Output the domain as a string
  void output(std::ostream &out, const ai_baset &ai, const namespacet &ns)
    const override;

  //Create a new dimension
  //dimension is a number of variables in scope
  //reason for seperating dimension from adding inequality
  //number of vars and dimensions should be the same after resolving all interm ops

  tvpi_systemt::dimensiont eval(exprt e, std::vector<std::string> &temporaries);
  void assume(exprt e);
  void assign(symbol_exprt lhs, exprt e);

protected:
  //the TVPI system object
  tvpi_systemt sys;
  //variable binded to some dimension
  //provides seperation between abstract program and program variables
  std::map<symbol_exprt, tvpi_systemt::dimensiont> binding;
  //track the latest dimension
  mp_integer dimension_counter;
  //example binding
  //int x;
  //binding = {{x,0}};
  //int y;
  //binding = {{x,0},{y,1}};

  //the number of mathematical dimensions is 2
  //the number of program variables is 2
};

#endif
