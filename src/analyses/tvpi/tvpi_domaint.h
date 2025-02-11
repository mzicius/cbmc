#ifndef TVPI_DOMAINT_H
#define TVPI_DOMAINT_H

#include <util/mp_arith.h>
#include <solvers/tvpi/tvpi_systemt.h>
#include <vector>

#include "../ai_domain.h"

class tvpi_domaint : public ai_domain_baset
{
public:

  typedef std::map<symbol_exprt, tvpi_systemt::dimensiont> binding_map;

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

  tvpi_systemt::dimensiont eval(exprt e);
  void assume(exprt e);
  void assign(symbol_exprt lhs, exprt e);
  tvpi_systemt::dimensiont lookup_binding(symbol_exprt sym);

protected:
  //the TVPI system object
  tvpi_systemt sys;
  //variable binded to some dimension
  //provides seperation between abstract program and program variables
  binding_map binding;
  //track the latest dimension
  mp_integer dimension_counter;
  //example binding
  //int x;
  //binding = {{x,0}};
  //int y;
  //binding = {{x,0},{y,1}};
  std::map<symbol_exprt, mp_integer> references;

  //the number of mathematical dimensions is 2
  //the number of program variables is 2
};

extern symbol_exprt str2symex(const std::string &label);
extern std::vector<std::shared_ptr<inequality>> relabel_ineqs(tvpi_systemt &sys, tvpi_systemt::dimensiont &old_dim, const tvpi_systemt::dimensiont &new_dim);
extern void align_bindings(const tvpi_domaint::binding_map &left, tvpi_domaint::binding_map &right,const tvpi_systemt &a, tvpi_systemt &b);
extern std::set<std::string> find_relations(const tvpi_systemt &a, const tvpi_systemt &b);
extern std::vector<std::shared_ptr<inequality>> filter(const tvpi_systemt &sys,std::vector<std::string> &target_vars);
extern std::vector<std::shared_ptr<inequality>> add_var(tvpi_systemt sys);
#endif
