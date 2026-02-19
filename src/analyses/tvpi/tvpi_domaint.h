#ifndef TVPI_DOMAINT_H
#define TVPI_DOMAINT_H

#include <util/mp_arith.h>
#include <solvers/tvpi/tvpi_systemt.h>
#include "tvpi_bindingt.h"
#include <vector>
#include "../variable-sensitivity/abstract_environment.h"
#include "../ai_domain.h"

/*

#define OPT_TVPI     \
  "(tvpi-arr-mode):"  \

#define PARSE_OPTIONS_TVPI(cmdline, options)                          \
    options.set_option("arr-mode", cmdline.get_value("tvpi-arr-mode"));  \
    arr_flag = cmdline.get_value("tvpi-arr-mode"); \
    (void)0

*/


class tvpi_domaint : public ai_domain_baset
{
public:


  //array options
  static std::string arr_mode;

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
  bool merge(const tvpi_domaint &b, trace_ptrt from, trace_ptrt to);

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

  tvpi_systemt::dimensiont eval(exprt e);

  void assume(const exprt &e);

  void assign(symbol_exprt lhs, exprt e);
  
  virtual bool ai_simplify(
    exprt &condition,
    const namespacet &ns) const override;

  //loop fix
  int loop_round;

protected:
  tvpi_systemt sys;
  tvpi_bindingt bind;
};

extern symbol_exprt str2symex(const std::string &label);
extern std::vector<std::shared_ptr<inequality>> relabel_ineqs(tvpi_systemt &sys, tvpi_systemt::dimensiont &old_dim, const tvpi_systemt::dimensiont &new_dim);
extern void align_bindings(tvpi_bindingt::binding_map &left,const tvpi_bindingt::binding_map &right,tvpi_systemt &a,const tvpi_systemt &b);
extern std::set<std::string> find_relations(const tvpi_systemt &a, const tvpi_systemt &b);

#endif
