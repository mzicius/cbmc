#include "tvpi_domaint.h"

#include <util/arith_tools.h>
#include <util/mp_arith.h>

#include <solvers/tvpi/join.h>
#include <solvers/tvpi/smt_printer.h>
#include <solvers/tvpi/tikz_printer.h>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>

tvpi_domaint::tvpi_domaint()
{
  // This must set the domain to bottom i.e. "no possible values"
  std::cerr << "Constructing domain" << std::endl;
  make_bottom();
}

tvpi_domaint::~tvpi_domaint()
{
  // You probably don't need anything here
}

void tvpi_domaint::make_bottom()
{
  //reach flag
  //this->may_reach = false;
  //unsat, does not represent any points
  sys.make_unsat_system();
}

void tvpi_domaint::make_top()
{
  //reach flag
  //this->may_reach = true;
  //describes everything
  sys = tvpi_systemt();
  bind = tvpi_bindingt();
}

void tvpi_domaint::make_entry()
{
  // This is fine for almost everyone
  make_top();
}

bool tvpi_domaint::is_bottom() const
{
  bool result = false;
  if(sys.constraints.size() > 0)
  {
    std::shared_ptr<inequality> ineq = sys.constraints[0];
    if(std::dynamic_pointer_cast<constant_inequality>(ineq) != nullptr)
    {
      std::shared_ptr<constant_inequality> c =
        std::dynamic_pointer_cast<constant_inequality>(ineq);
      if(c->sat == false)
      {
        result = true;
      }
    }
  }
  return result;
}

bool tvpi_domaint::is_top() const
{
  return this->sys.constraints.size() == 0;
}

void tvpi_domaint::output(
  std::ostream &out,
  const ai_baset &ai,
  const namespacet &ns) const
{
  out << std::endl;
  out << "dimension counter: " << this->sys.get_current_dim() << std::endl;
  out << std::endl;
  out << "TVPI system:" << std::endl;
  print_cons(this->sys.constraints);
  out << std::endl;
  std::cout << "the binding is: " << std::endl;
  this->bind.print_binding();
  out << std::endl;
  std::cout << "the references are: " << std::endl;
  this->bind.print_references();
}

//Create a dimension in the TVPI-system that over-approximates
//the value of an expression.
tvpi_systemt::dimensiont tvpi_domaint::eval(exprt e)
{
  std::cerr << "inside the eval function" << std::endl;
  //std::cerr << "evaluating expression: " << e.pretty() << std::endl;
  //std::cerr << "evaluating expression: " << e.get_string() << std::endl;

  if(e.id() == ID_constant)
  {
    std::cout << "we met a constant" << std::endl;
    tvpi_systemt::dimensiont c = this->sys.add_new_dimension();
    mp_integer const_e = numeric_cast_v<mp_integer>(to_constant_expr(e));
    this->sys.add_inequality(1, "d" + integer2string(c), 0, "d", const_e);
    this->sys.add_inequality(-1, "d" + integer2string(c), 0, "d", -const_e);
    return c;
  }
  else if(e.id() == ID_symbol)
  {
    symbol_exprt symbol = to_symbol_expr(e);
    tvpi_systemt::dimensiont result_dim = this->bind.lookup_binding(symbol);

    if(result_dim >= 0)
    {
      std::cout << "in eval we found: " << symbol.get_identifier()
                << " at: " << result_dim << std::endl;
      return result_dim;
    }
    else
    {
      result_dim = this->sys.add_new_dimension();
      this->bind.set_binding(symbol, result_dim);
      return result_dim;
    }
  }
  else if(e.id() == ID_plus)
  {
    std::cout << "we are in plus" << std::endl;
    tvpi_systemt::dimensiont sum_dim = this->sys.add_new_dimension();
    this->bind.add_tmp_ref(sum_dim);
    plus_exprt plus_e = to_plus_expr(e);

    //TODO: do constant folding
    if(plus_e.op0().is_constant() && plus_e.op1().is_constant())
    {
      mp_integer const_left =
        numeric_cast_v<mp_integer>(to_constant_expr(plus_e.op0()));
      mp_integer const_right =
        numeric_cast_v<mp_integer>(to_constant_expr(plus_e.op1()));
      this->sys.add_inequality(
        1, "d" + integer2string(sum_dim), 0, "d", const_left + const_right);
      this->sys.add_inequality(
        -1, "d" + integer2string(sum_dim), 0, "d", -(const_left + const_right));
    }

    tvpi_systemt::dimensiont left = eval(plus_e.op0());
    tvpi_systemt::dimensiont right = eval(plus_e.op1());

    std::optional<rationalt> u_bound_left = this->sys.get_ub(left);
    std::optional<rationalt> l_bound_left = this->sys.get_lb(left);
    std::optional<rationalt> u_bound_right = this->sys.get_ub(right);
    std::optional<rationalt> l_bound_right = this->sys.get_lb(right);

    if(u_bound_right.has_value())
    {
      std::cout << "u_bound_right: " << u_bound_right.value() << std::endl;
    }

    if(u_bound_left.has_value())
    {
      std::cout << "u_bound_left: " << u_bound_left.value() << std::endl;
    }

    if(l_bound_right.has_value())
    {
      std::cout << "l_bound_right: " << l_bound_right.value() << std::endl;
    }

    if(l_bound_left.has_value())
    {
      std::cout << "l_bound_left: " << l_bound_left.value() << std::endl;
    }

    //Theorem 1
    if(u_bound_right.has_value())
    {
      this->sys.add_inequality(
        rationalt(-1),
        "d" + integer2string(left),
        rationalt(1),
        "d" + integer2string(sum_dim),
        u_bound_right.value());
    }

    if(u_bound_left.has_value())
    {
      this->sys.add_inequality(
        rationalt(-1),
        "d" + integer2string(right),
        rationalt(1),
        "d" + integer2string(sum_dim),
        u_bound_left.value());
    }

    //Theorem 2
    if(l_bound_right.has_value())
    {
      this->sys.add_inequality(
        rationalt(1),
        "d" + integer2string(left),
        rationalt(-1),
        "d" + integer2string(sum_dim),
        -l_bound_right.value());
    }

    if(l_bound_left.has_value())
    {
      this->sys.add_inequality(
        rationalt(1),
        "d" + integer2string(right),
        rationalt(-1),
        "d" + integer2string(sum_dim),
        -l_bound_left.value());
    }

    //Theorem 3
    //Theorem 4
    //Theorem N

    return sum_dim;
  }
  else if(e.id() == ID_unary_minus)
  {
    std::cerr << "inside unary minus" << std::endl;
    unary_minus_exprt u_min_e = to_unary_minus_expr(e);

    tvpi_systemt::dimensiont left = eval(u_min_e.op());
    tvpi_systemt::dimensiont c = this->sys.add_new_dimension();
    this->bind.add_tmp_ref(left);
    this->bind.add_tmp_ref(c);

    std::cout << "unary - new d: " << c << " old: " << left << std::endl;

    // new_dim  - eval_result <= 0
    //-new_dim  + eval_result <= 0

    this->sys.add_inequality(
      -1, "d" + integer2string(c), -1, "d" + integer2string(left), 0);
    this->sys.add_inequality(
      1, "d" + integer2string(c), 1, "d" + integer2string(left), 0);

    return c;
  }
  else if(e.id() == ID_minus)
  {
    std::cout << "we are in ID minus" << std::endl;
    binary_exprt bin_exp = to_binary_expr(e);
    return eval(plus_exprt(bin_exp.lhs(), unary_minus_exprt(bin_exp.rhs())));
  }
  else
  {
    std::cerr << "No evaluation for e:" << id2string(e.id()) << std::endl;
    return -1;
  }
  UNREACHABLE;
  /*
  else if(e.id() == ID_mult)
  {
    tvpi_systemt::dimensiont mult_dim = this->sys.add_new_dimension();
    mult_exprt mult_e = to_mult_expr(e);
    tvpi_systemt::dimensiont left =
      eval(to_symbol_expr(mult_e.op0()));
    tvpi_systemt::dimensiont right =
      eval(to_symbol_expr(mult_e.op1()));

    std::optional<mp_integer> u_bound_left = this->sys.get_ub(left);
    std::optional<mp_integer> l_bound_left = this->sys.get_lb(left);
    std::optional<mp_integer> u_bound_right = this->sys.get_ub(right);
    std::optional<mp_integer> l_bound_right = this->sys.get_lb(right);

    //Theorem 1
    if(u_bound_left.has_value() && u_bound_right.has_value())
    {
      this->sys.add_inequality(
        1,
        "d" + integer2string(mult_dim),
        -1 * u_bound_right.value(),
        "d" + integer2string(left),
        mp_integer(0));
      this->sys.add_inequality(
        1,
        "d" + integer2string(mult_dim),
        -1 * u_bound_left.value(),
        "d" + integer2string(right),
        mp_integer(0));
    }

    std::cerr << "left: " << left << " right: " << right << std::endl;
    return mult_dim;
  }*/
}

// Takes a bool expression and reduces the domain
// to an overapproximation of when this condition holds
void tvpi_domaint::assume(const exprt &e)

{
  std::cout << "inside the assume function" << std::endl;

  //std::cout << "assuming expression: " << e.pretty() << std::endl;
  //std::cout << "assuming expression: " << e.type() << std::endl;

  if(e.id() == ID_equal)
  {
    //turn to object and then recurse
    //cant link inequalities due to l and r being numbers and not strings
    //how can the map be examinded for keys what if two keys (vars) hold the same dimensions

    tvpi_systemt::dimensiont l = eval(to_equal_expr(e).lhs());
    tvpi_systemt::dimensiont r = eval(to_equal_expr(e).rhs());

    std::string label_l, label_r;
    label_l = "d" + integer2string(l);
    label_r = "d" + integer2string(r);

    std::cout << "ID equal -> label left: " << label_l
              << " label right: " << label_r << std::endl;

    sys.add_inequality(1, label_l, -1, label_r, 0);
    sys.add_inequality(-1, label_l, 1, label_r, 0);

    return;
  }
  if(e.id() == ID_le)
  {
    auto bin = to_binary_expr(e);
    tvpi_systemt::dimensiont l = eval(bin.lhs());
    tvpi_systemt::dimensiont r = eval(bin.rhs());

    std::string label_l, label_r;
    label_l = "d" + integer2string(l);
    label_r = "d" + integer2string(r);

    std::cout << "less or equal -> label left: " << label_l
              << " label right: " << label_r << std::endl;
    sys.add_inequality(1, label_l, -1, label_r, 0);

    return;
  }
  if(e.id() == ID_lt)
  {
    std::cout << "we are in the less than" << std::endl;
    auto bin = to_binary_expr(e);
    std::cout << "before left " << this->sys.constraints.size() << std::endl;
    tvpi_systemt::dimensiont l = eval(bin.lhs());
    std::cout << "my left label is: " << l << std::endl;
    std::cout << "before right " << this->sys.constraints.size() << std::endl;
    tvpi_systemt::dimensiont r = eval(bin.rhs());
    std::cout << "final sys " << this->sys.constraints.size() << std::endl;
    std::cout << "my right label is: " << r << std::endl;
    this->sys.add_inequality(
      1, "d" + integer2string(l), -1, "d" + integer2string(r), -1);
    return;
  }
  if(e.id() == ID_ge)
  {
    auto bin = to_binary_expr(e);
    tvpi_systemt::dimensiont l = eval(bin.lhs());
    tvpi_systemt::dimensiont r = eval(bin.rhs());

    std::string label_l, label_r;
    label_l = "d" + integer2string(l);
    label_r = "d" + integer2string(r);

    std::cout << "greater or equal -> label left: " << label_l
              << " label right: " << label_r << std::endl;
    sys.add_inequality(-1, label_l, 1, label_r, 0);
    return;
  }
  if(e.id() == ID_gt)
  {
    auto bin = to_binary_expr(e);
    tvpi_systemt::dimensiont l = eval(bin.lhs());
    tvpi_systemt::dimensiont r = eval(bin.rhs());

    std::string label_l, label_r;
    label_l = "d" + integer2string(l);
    label_r = "d" + integer2string(r);

    std::cout << "greater -> label left: " << label_l
              << " label right: " << label_r << std::endl;
    sys.add_inequality(-1, label_l, 1, label_r, -1);
    return;
  }
  if(e.id() == ID_and)
  {
    assume(to_and_expr(e).op0());
    assume(to_and_expr(e).op1());
    return;
  }
  if(e.id() == ID_or)
  {
    // These requires some magic
    // Don't worry about it for now.
  }
  if(e.id() == ID_not)
  {
    //std::cout << "we are in not" <<e.get_string()<<std::endl;

    //std::cout << e.pretty() << std::endl;
    not_exprt tmp(to_not_expr(e));
    if(tmp.op().id() == ID_not)
    {
      assume(to_not_expr(tmp.op()).op());
    }
    else if(tmp.op().id() == ID_lt)
    {
      auto rel = to_binary_relation_expr(tmp.op());
      assume(greater_than_or_equal_exprt(rel.lhs(), rel.rhs()));
    }
    else if(tmp.op().id() == ID_le)
    {
      auto rel = to_binary_relation_expr(tmp.op());
      assume(greater_than_exprt(rel.lhs(), rel.rhs()));
    }
    else if(tmp.op().id() == ID_gt)
    {
      auto rel = to_binary_relation_expr(tmp.op());
      assume(less_than_or_equal_exprt(rel.lhs(), rel.rhs()));
    }
    else if(tmp.op().id() == ID_ge)
    {
      auto rel = to_binary_relation_expr(tmp.op());
      assume(less_than_exprt(rel.lhs(), rel.rhs()));
    }
    else if(tmp.op().id() == ID_equal)
    {
      std::cerr << "We are def checking this" << std::endl;
      //fix this
      //because this is the wrong theorem
      auto rel = to_binary_relation_expr(tmp.op());
      assume(equal_exprt(rel.lhs(), rel.rhs()));
    }
    return;
  }
  std::cerr << "If only I knew how to assume a " << id2string(e.id())
            << std::endl;
}

// Updates the binding of the symbol to point to a dimension
// that over-approximates e
void tvpi_domaint::assign(symbol_exprt lhs, exprt e)
{
  std::cerr << "in asign with: " << lhs.get_identifier() << std::endl;
  tvpi_systemt::dimensiont evaluated_dim = eval(e);
  bind.set_binding(lhs, evaluated_dim);
}

bool tvpi_domaint::ai_simplify(exprt &condition, const namespacet &ns) const
{
  std::cout << "ai_simplify for tvpi!" << std::endl;
  //std::cout << "the condition is: " << condition.pretty() << std::endl;
  bool unchanged = true;
  tvpi_domaint copy_a(*this);
  tvpi_domaint copy_b(*this);

  if(condition.id() == ID_symbol)
  {
    std::cerr << "trying to simplify ID_symbol" << std::endl;
  }
  else if(condition.id() == ID_and)
  {
    std::cerr << "trying to simplify ID_and" << std::endl;
  }
  else
  {
    std::cerr << "trying to simplify" << std::endl;

    copy_a.assume(condition);
    std::cerr << "trying CASE A" << std::endl;
    if(copy_a.is_bottom())
    {
      std::cerr << "bottom in CASE A" << std::endl;
      condition = false_exprt();
    }
    else
    {
      std::cerr << "trying CASE B" << std::endl;
      copy_b.assume(not_exprt(condition));
      if(copy_b.is_bottom())
      {
        std::cerr << "bottom in CASE B" << std::endl;
        condition = true_exprt();
        unchanged = false;
      }
    }
  }

  return unchanged;
}

//int tvpi_domaint::result_call;
//std::chrono::milliseconds tvpi_domaint::total_duration;
//std::size_t tvpi_domaint::input_sys_size;
//std::size_t tvpi_domaint::output_sys_size;

// Transform updates the domain with the effect of the instruction "from"
void tvpi_domaint::transform(
  const irep_idt &function_from,
  trace_ptrt from,
  const irep_idt &function_to,
  trace_ptrt to,
  ai_baset &ai,
  const namespacet &ns)
{
  this->sys.print_system();

  std::cerr << "TVPI domain @ 0x" << this << " transform using instruction "
            << from->current_location()->location_number << '\n';

  const goto_programt::instructiont &instruction = *(from->current_location());
  std::cerr << "The instruction type is: " << instruction.to_string()
            << std::endl;

  switch(instruction.type())
  {
  case DECL:
    this->bind.set_binding(
      to_code_decl(instruction.code()).symbol(), this->sys.add_new_dimension());
    break;

  case DEAD:
    //existential project
    //reduce refs
    break;

  case ASSIGN:
  {
    exprt e = instruction.assign_lhs();
    if(e.id() == ID_symbol)
    {
      assign(to_symbol_expr(e), instruction.assign_rhs());
    }
    else if(e.id() == ID_dereference)
    {
      //int *p = 0;
      //The pointer address can point to the following
      //CASE 1 - some object
      //CASE 2 - location immediately past the object
      //CASE 3 - null pointer
      //CASE 4 - invalid value
    }
    break;
  }
  case GOTO:
  {
    // Comparing iterators is safe as the target must be within the same list
    // of instructions because this is a GOTO.
    std::cout << "inside GOTO" << std::endl;
    locationt next = from->current_location();
    next++;
    if(
      from->current_location()->get_target() !=
      next) // If equal then effectively a SKIP
    {
      if(next == to->current_location())
      {
        // Branch is not taken
        std::cout << "branch not taken" << std::endl;
        assume(not_exprt(instruction.condition()));
      }
      else
      {
        // Branch is taken
        std::cout << "branch taken" << std::endl;
        assume(instruction.condition());
      }
    }
    break;
  }

  case FUNCTION_CALL:
  {
    // Function calls are a bit of a fiddle...
    //const code_function_callt &code_function_call =
    //to_code_function_call(instruction.code());
    //std::cout<<"code_function_call"<<code_function_call.pretty()<<std::endl;
    //code_function_call.

    const exprt &function = instruction.call_function();

    if(function.id() == ID_symbol)
    {
      const irep_idt &identifier = to_symbol_expr(function).get_identifier();
      if(identifier == "assume")
      {
        std::cout << "yes yes yes" << std::endl;
      }
    }

    break;
  }
  /*  Removed from more recent versions
  case RETURN:  // Are transformed away into SET_RETURN_VALUE
                // and then GOTO the end of the function
    DATA_INVARIANT(false, "Returns must be removed before analysis");
    break;
  */
  case SET_RETURN_VALUE:
  {
    // Handle setting the return value
    break;
  }

  //__CPROVER_ASSUME
  case ASSUME:
    assume(instruction.condition());
    break;
    /** These are instructions you really can ignore **/
  case ASSERT: // An assert is a check; they don't alter execution
    // If goto-analyzer is run with --verify they will be checked after fixpoint
    // by using the ai_simplify() method given below.
    break;

    // Mostly markers
  case LOCATION:     // No action required
  case SKIP:         // No action required
  case END_FUNCTION: // No action required
    break;

  case CATCH:
  case THROW:
    DATA_INVARIANT(false, "Exceptions must be removed before analysis");
    break;

  case ATOMIC_BEGIN: // Ignoring is a valid over-approximation
  case ATOMIC_END:   // Ignoring is a valid over-approximation
  case START_THREAD: // Require a concurrent analysis at higher level
  case END_THREAD:   // Require a concurrent analysis at higher level
    break;

  case OTHER:
#if 0
    DATA_INVARIANT(false, "Unclear what is a safe over-approximation of OTHER");
#endif
    break;

  case INCOMPLETE_GOTO:
  case NO_INSTRUCTION_TYPE:
    DATA_INVARIANT(false, "Only complete instructions can be analyzed");
    break;
  }

  //garbage collection
  this->bind.wipe_binding(this->sys);

  return;
}

//trace_ptrt can be seen as location, will be used for widening
bool tvpi_domaint::merge(const tvpi_domaint &b, trace_ptrt from, trace_ptrt to)
{
  // nothing to do
  if(b.is_bottom())
  {
    std::cerr << "MERGE CASE 1: B IS BOTTOM" << std::endl;
    return false;
  }

  // copy of constrains, binding, ref_counter, dim_counter of system b
  if(this->is_bottom())
  {
    INVARIANT(!b.is_bottom(), "CASE HANDLED");
    std::cerr << "MERGE CASE 2: A IS BOTTOM" << std::endl;
    this->sys.constraints = b.sys.constraints;
    this->bind = b.bind;
    return true;
  }

  INVARIANT(!this->is_bottom() && !b.is_bottom(), "CASE HANDLED");

  bool is_modified = false;

  // handle top
  if(b.is_top())
  {
    std::cerr << "MERGE CASE 3: B IS TOP" << std::endl;

    // change if it was not top
    is_modified = !this->is_top();

    make_top();

    return is_modified;
  }

  std::cerr << "MERGE CASE 4: CONVEX UNION" << std::endl;

  //copy binding of systems a and b
  tvpi_bindingt::binding_map copy_left = this->bind.copy_map();
  tvpi_bindingt::binding_map copy_right = b.bind.copy_map();

  //copy systems a and b
  tvpi_systemt copy_a = this->sys;
  tvpi_systemt copy_b = b.sys;

  std::cerr << "the left system for CONVEX UNION:" << std::endl;
  print_cons(this->sys.constraints);

  std::cerr << "the left binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : this->bind.binding)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  std::cerr << "the right system for CONVEX UNION:" << std::endl;
  print_cons(b.sys.constraints);

  std::cerr << "the right binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : b.bind.binding)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  align_bindings(this->bind.binding, b.bind.binding, this->sys, b.sys);

  std::cerr << "after align: left system for CONVEX UNION:" << std::endl;
  print_cons(this->sys.constraints);

  std::cerr << "after align: left binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : this->bind.binding)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  std::cerr << "after align: right system for CONVEX UNION:" << std::endl;
  print_cons(b.sys.constraints);

  std::cerr << "after align: right binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : b.bind.binding)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  std::set<std::string> existing_relations = find_relations(this->sys, b.sys);

  std::vector<std::shared_ptr<inequality>> interm_union;
  std::vector<std::shared_ptr<inequality>> convex_union;

  for(const auto &rel : existing_relations)
  {
    sweep_dimensions();
    std::vector<std::shared_ptr<inequality>> filter_left;
    std::vector<std::shared_ptr<inequality>> filter_right;
    std::string label;
    std::vector<std::string> target_vars;
    auto dash = rel.find("-");
    std::string first_var = rel.substr(0, dash);
    std::string second_var = rel.substr(dash + 1);

    //std::cout << first_var << " part2: " << second_var << std::endl;
    if(first_var != second_var)
    {
      //std::cout << "first var: " << first_var << " second var: " << second_var
      //          << std::endl;
      target_vars = {first_var, second_var};
    }
    else
    {
      target_vars = {first_var};
    }

    filter_left = this->sys.filter(target_vars);
    filter_right = b.sys.filter(target_vars);

    /*
    std::cout << "the target vars for filter are: :" << std::endl;
    for(const auto &var : target_vars)
    {
      std::cout << var << std::endl;
    }
    */

    //std::cout << "left filter" << std::endl;
    //print_cons(filter_left);
    //std::cout << "right filter" << std::endl;
    //print_cons(filter_right);

    extract_dimensions(filter_left);
    extract_dimensions(filter_right);

    interm_union = join::calc_hull(filter_left, filter_right);

    std::cout << "inter convex hull is: " << std::endl;
    print_cons(interm_union);
    convex_union.insert(
      convex_union.end(), interm_union.begin(), interm_union.end());
  }

  //check if the new system is different
  std::cout << "the convex hull is:" << std::endl;
  print_cons(convex_union);

  auto widen_mode =
    from->should_widen(*to) ? widen_modet::could_widen : widen_modet::no;

  if(
    widen_mode == widen_modet::could_widen &&
    from->current_location()->is_backwards_goto() &&
    from->current_location()->get_target() == to->current_location())
  {
    std::cerr << "MERGE CASE 4: WIDEN" << std::endl;
    auto copy_a = this->sys.constraints;
    auto copy_b = b.sys.constraints;

    std::vector<std::shared_ptr<inequality>> intersection;

    std::cout << "for inter left: " << std::endl;
    print_cons(convex_union);

    std::cout << "for inter right: " << std::endl;
    print_cons(this->sys.constraints);

    /*
    for(auto con_a : convex_union)
    {
      for(auto con_b : this->sys.constraints)
      {
        if(con_a->to_string() == con_b->to_string())
        {
          intersection.push_back(con_a);
        }
      }
  

    }
    */

    intersection = this->sys.intersect(convex_union);

    /*
    std::cerr << "intersection is:" << std::endl;
    print_cons(intersection);

    std::cout << "the main flow system now is: " << std::endl;
    print_cons(this->sys.constraints);

    std::cout << "is_equal inter to main: "
              << std::equal(
                   intersection.begin(),
                   intersection.end(),
                   this->sys.constraints.begin())
              << std::endl;
    */

    //cautious
    //should go at least twice around the loop before starting widening
    //control flow
    //check the flags
    //in two points
    //print flags
    //custom check for equiv of containers
    //if(intersection != this->sys.constraints){
    //the size dif dif systems then

    //if(!std::equal(
    //intersection.begin(),
    //this->sys.constraints.begin()))

    if(!this->sys.is_equal(intersection))
    {
      std::cerr << "system is updated with inter" << std::endl;
      this->sys.constraints = intersection;
      is_modified = true;
    }
  }
  else
  {
    /*
    std::cout << "the convex union is: " << std::endl;
    print_cons(convex_union);

    std::cout << "the main flow system is: " << std::endl;
    print_cons(this->sys.constraints);

    std::cout << "is_equal convex to main: "
              << std::equal(
                   convex_union.begin(),
                   convex_union.end(),
                   this->sys.constraints.begin())
              << std::endl;
    */
    //!std::equal(
    // convex_union.begin(),
    // convex_union.end(),
    // this->sys.constraints.begin())

    if(!this->sys.is_equal(convex_union))
    {
      std::cerr << "system is updated with convex union" << std::endl;
      this->sys.constraints = convex_union;
      is_modified = true;
    }
  }

  return is_modified;
}

std::vector<std::shared_ptr<inequality>> relabel_ineqs(
  tvpi_systemt &sys,
  tvpi_systemt::dimensiont &old_dim,
  const tvpi_systemt::dimensiont &new_dim)
{
  std::vector<std::shared_ptr<inequality>> res;
  for(std::shared_ptr<inequality> c : sys.constraints)
  {
    std::vector<std::string> vars = c->vars();

    auto var_pos =
      find(vars.begin(), vars.end(), "d" + integer2string(old_dim));

    if(var_pos != vars.end())
    {
      if(cast_to_unary(c))
      {
        std::shared_ptr<unary_inequality> u =
          std::dynamic_pointer_cast<unary_inequality>(c);
        //fix  std::make_shared<dyadic_inequality>

        std::shared_ptr<inequality> new_u = inequality_factory::make_inequality(
          "d" + integer2string(new_dim), "", u->a, 0, u->c);
        res.push_back(new_u);
      }
      else if(cast_to_dyadic(c))
      {
        std::shared_ptr<dyadic_inequality> d =
          std::dynamic_pointer_cast<dyadic_inequality>(c);
        std::shared_ptr<inequality> new_d;
        if(d->x == "d" + integer2string(old_dim))
        {
          new_d = inequality_factory::make_inequality(
            "d" + integer2string(new_dim), d->y, d->a, d->b, d->c);
        }
        else
        {
          new_d = inequality_factory::make_inequality(
            d->x, "d" + integer2string(new_dim), d->a, d->b, d->c);
        }
        res.push_back(new_d);
      }
    }
    else
    {
      res.push_back(c);
    }
  }
  return res;
}

void align_bindings(
  tvpi_bindingt::binding_map &left,
  const tvpi_bindingt::binding_map &right,
  tvpi_systemt &a,
  const tvpi_systemt &b)
{
  for(const auto &right_bind_pair : right)
  {
    auto loc_in_left = left.find(right_bind_pair.first);

    if(
      loc_in_left != left.end() &&
      loc_in_left->second != right_bind_pair.second)
    {
      a.constraints =
        relabel_ineqs(a, loc_in_left->second, right_bind_pair.second);
      left[right_bind_pair.first] = right_bind_pair.second;
    }
    //fix the refrences in here
    //think about the location
  }
}

symbol_exprt str2symex(const std::string &label)
{
  return symbol_exprt(irep_idt(label), typet());
}

std::set<std::string>
find_relations(const tvpi_systemt &a, const tvpi_systemt &b)
{
  std::set<std::string> found;
  for(auto i : a.constraints)
  {
    if(i->arity() == 1)
    {
      auto var = i->vars()[0];
      auto loc = found.find(var);
      if(loc == found.end())
      {
        found.insert(var);
      }
    }
    else if(i->arity() == 2)
    {
      auto vars = i->vars();
      std::string label = vars[0] + "-" + vars[1];
      std::string rev_label = vars[1] + "-" + vars[0];
      auto loc = found.find(label);
      if(loc == found.end())
      {
        found.insert(label);
      }
    }
  }
  //iterate b
  for(auto i : b.constraints)
  {
    if(i->arity() == 1)
    {
      auto var = i->vars()[0];
      auto loc = found.find(var);
      if(loc == found.end())
      {
        found.insert(var);
      }
    }
    else if(i->arity() == 2)
    {
      auto vars = i->vars();
      std::string label = vars[0] + "-" + vars[1];
      std::string rev_label = vars[1] + "-" + vars[0];
      auto loc = found.find(label);
      if(loc == found.end())
      {
        found.insert(label);
      }
    }
  }
  return found;
}
