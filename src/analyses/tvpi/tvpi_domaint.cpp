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
  out << "dimension counter: " << this->sys.dimension_counter << std::endl;
  out << std::endl;

  out << "binding: " << std::endl;
  for(const auto &[symbol, dimension] : binding)
  {
    out << id2string(symbol.get_identifier()) << "-> " << dimension
        << std::endl;
  }
  out << std::endl;
  out << "TVPI system:" << std::endl;
  for(auto i : this->sys.constraints)
  {
    out << i->to_string() << std::endl;
  }
  out << std::endl;
  out << "the references are: " << std::endl;
  for(const auto &ref : this->sys.references)
  {
    if(ref.first >= 0)
    {
      out << "d" << ref.first << " -> " << ref.second << std::endl;
    }
    else
    {
      out << "-d" << abs(ref.first) << " -> " << ref.second << std::endl;
    }
  }
}

//Create a dimension in the TVPI-system that over-approximates
//the value of an expression.
tvpi_systemt::dimensiont tvpi_domaint::eval(exprt e)
{
  std::cerr << "inside the eval function" << std::endl;
  std::cerr << "evaluating expression: " << e.pretty() << std::endl;

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
    std::cout << "we looked up" << lookup_binding(to_symbol_expr(e))
              << std::endl;
    return lookup_binding(to_symbol_expr(e));
  }
  else if(e.id() == ID_plus)
  {
    std::cout << "we are in plus" << std::endl;
    tvpi_systemt::dimensiont sum_dim = this->sys.add_new_dimension();
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

    std::optional<mp_integer> u_bound_left = this->sys.get_ub(left);
    std::optional<mp_integer> l_bound_left = this->sys.get_lb(left);
    std::optional<mp_integer> u_bound_right = this->sys.get_ub(right);
    std::optional<mp_integer> l_bound_right = this->sys.get_lb(right);

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
        -1,
        "d" + integer2string(left),
        1,
        "d" + integer2string(sum_dim),
        u_bound_right.value());
    }

    if(u_bound_left.has_value())
    {
      this->sys.add_inequality(
        -1,
        "d" + integer2string(right),
        1,
        "d" + integer2string(sum_dim),
        u_bound_left.value());
    }

    //Theorem 2
    if(l_bound_right.has_value())
    {
      this->sys.add_inequality(
        1,
        "d" + integer2string(left),
        -1,
        "d" + integer2string(sum_dim),
        -l_bound_right.value());
    }

    if(l_bound_left.has_value())
    {
      this->sys.add_inequality(
        1,
        "d" + integer2string(right),
        -1,
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
    std::cout << "we are in unary minus" << std::endl;
    unary_minus_exprt u_min_e = to_unary_minus_expr(e);

    tvpi_systemt::dimensiont left = eval(u_min_e.op());

    tvpi_systemt::dimensiont c = this->sys.add_new_dimension();
    std::cout << "new dim is: " << c << " old dim is" << left << std::endl;

    //new_dim - eval_result <= 0
    //-new_dim  + eval_result <= 0

    this->sys.add_inequality(
      -1, "d" + integer2string(c), -1, "d" + integer2string(left), 0);
    this->sys.add_inequality(
      1, "d" + integer2string(c), 1, "d" + integer2string(left), 0);

    /*
    mp_integer const_e;
    if(u_min_e.op().id()==ID_constant){
     const_e = numeric_cast_v<mp_integer>(to_constant_expr(u_min_e.op()));
     std::cout<<"const_e now:"<<const_e<<std::endl;
     this->sys.add_inequality(1, "d" + integer2string(c), 0, "d", -const_e);
     this->sys.add_inequality(-1, "d" + integer2string(c), 0, "d", const_e);
    }
    else if(u_min_e.op().id()==ID_symbol){
     std::cout<<"we are in ID"<<std::endl;
     symbol_exprt s = to_symbol_expr(u_min_e.op());
     tvpi_systemt::dimensiont binded_dim = lookup_binding(s);
     std::cout<<"the binded dimension we found is:"<<binded_dim<<std::endl;
    
    if(binded_dim<0){
      binded_dim = c;
    }

    auto ub = this->sys.get_ub(binded_dim);
    auto lb = this->sys.get_lb(binded_dim);

    if(ub.has_value()&& lb.has_value()){
    std::cout<<"ub is: "<<ub.value()<<"lb is: "<<lb.value()<<std::endl;
    this->sys.add_inequality(1, "d" + integer2string(c), 0, "d", lb.value());
    this->sys.add_inequality(-1, "d" + integer2string(c), 0, "d", ub.value());
    }
  
    }
    */

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
  std::cout << "assuming expression: " << e.pretty() << std::endl;
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
    std::cout << "and now this" << std::endl;
    auto bin = to_binary_expr(e);
    tvpi_systemt::dimensiont l = eval(bin.lhs());
    //issue here?
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
    std::cout << "we are in not" << std::endl;
    std::cout << e.pretty() << std::endl;
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
      std::cout << "we hit this" << std::endl;
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
  std::cerr << "We are in assign with: " << id2string(lhs.get_identifier())
            << std::endl;

  tvpi_systemt::dimensiont evaluated_dim = eval(e);

  std::cout << "assigned dim: " << binding[lhs]
            << " evaluated dim: " << evaluated_dim << std::endl;

  //decrease the number of references if not dealing with return
  if(id2string(lhs.get_identifier()).find("return") == std::string::npos)
  {
    this->sys.references[binding[lhs]] = this->sys.references[binding[lhs]] - 1;
  }

  binding[lhs] = evaluated_dim;

  //increase the number of references
  this->sys.references[binding[lhs]] = this->sys.references[binding[lhs]] + 1;
}

bool tvpi_domaint::ai_simplify(exprt &condition, const namespacet &ns) const
{
  std::cout << "ai_simplify for tvpi!" << std::endl;
  std::cout << "the condition is: " << condition.pretty() << std::endl;
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
    this->sys.add_new_dimension();
    this->binding.insert(std::make_pair(
      to_code_decl(instruction.code()).symbol(), this->sys.dimension_counter));
    this->sys.references[binding[to_code_decl(instruction.code()).symbol()]] =
      this->sys.references[binding[to_code_decl(instruction.code()).symbol()]] +
      1;
    break;

  case DEAD:
    /*
    Project out the var, remove from binding and decrease the dimension_counter
    if(
      id2string(to_code_dead(instruction.code()).symbol().get_identifier())
        .find("return_value") == std::string::npos)
    {
    this->sys.existential_project(
      this->binding[to_code_dead(instruction.code()).symbol()]);
    this->binding.erase(to_code_dead(instruction.code()).symbol());
    this->sys.dimension_counter -= 1;
    }
    */
    break;

  case ASSIGN:
    //if(
      //id2string(to_symbol_expr(instruction.assign_lhs()).get_identifier())
        //.find("__CPROVER") == std::string::npos)
    //{
      assign(
        to_symbol_expr(instruction.assign_lhs()), instruction.assign_rhs());
    //}
    break;

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

  for(auto it = this->sys.references.begin(); it != this->sys.references.end();)
  {
    const auto &ref = *it;
    if(ref.second == 0)
    {
      sys.existential_project(ref.first);
      it = this->sys.references.erase(it);
    }
    else
    {
      ++it;
    }
  }

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
    this->binding = b.binding;
    this->sys.references = b.sys.references;
    this->sys.dimension_counter = b.sys.dimension_counter;
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

  std::cerr << "MERGE CASE 4: WIDEN OR CONVEX UNION" << std::endl;

  auto widen_mode =
    from->should_widen(*to) ? widen_modet::could_widen : widen_modet::no;

  if(widen_mode == widen_modet::could_widen)
  {
    std::cerr << "MERGE CASE 4: WIDEN" << std::endl;
    auto copy_a = this->sys.constraints;
    auto copy_b = b.sys.constraints;
    std::cerr << "system a " << std::endl;
    print_cons(copy_a);
    std::cerr << "system b " << std::endl;
    print_cons(copy_b);

    std::vector<std::shared_ptr<inequality>> intersection;

    for(auto con_a : copy_a)
    {
      for(auto con_b : copy_b)
      {
        if(con_a->to_string() == con_b->to_string())
        {
          intersection.push_back(con_a);
        }
      }
    }

    std::cerr << "intersection " << std::endl;
    print_cons(intersection);
    this->sys.constraints = intersection;
  }

  std::cerr << "MERGE CASE 4: CONVEX UNION" << std::endl;
  binding_map copy_left = this->binding;
  binding_map copy_right = b.binding;

  tvpi_systemt copy_a = this->sys;
  tvpi_systemt copy_b = b.sys;

  std::cerr << "the left system for CONVEX UNION:" << std::endl;
  print_cons(copy_a.constraints);

  std::cerr << "the left binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : copy_left)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  std::cerr << "the right system for CONVEX UNION:" << std::endl;
  print_cons(copy_b.constraints);

  std::cerr << "the right binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : copy_right)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  align_bindings(copy_left, copy_right, copy_a, copy_b);

  std::cerr << "after align: left system for CONVEX UNION:" << std::endl;
  print_cons(copy_a.constraints);

  std::cerr << "after align: left binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : copy_left)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  std::cerr << "after align: right system for CONVEX UNION:" << std::endl;
  print_cons(copy_b.constraints);

  std::cerr << "after align: right binding for CONVEX UNION:" << std::endl;
  for(const auto &bind_pair : copy_right)
  {
    std::cout << id2string(bind_pair.first.get_identifier()) << " -> "
              << bind_pair.second << std::endl;
  }

  std::set<std::string> existing_relations = find_relations(copy_a, copy_b);

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

    //std::cout << "vars here::" << std::endl;
    //print_ineq(copy_a.constraints[0]);

    filter_left = filter(copy_a, target_vars);
    filter_right = filter(copy_b, target_vars);
    std::cout << "the target vars for filter are: :" << std::endl;
    for(const auto &var : target_vars)
    {
      std::cout << var << std::endl;
    }

    std::cout << "left filter" << std::endl;
    print_cons(filter_left);
    std::cout << "right filter" << std::endl;
    print_cons(filter_right);
    extract_dimensions(filter_left);
    extract_dimensions(filter_right);
    interm_union = join::calc_hull(filter_left, filter_right);
    std::cout << "inter convex hull is: " << std::endl;
    print_cons(interm_union);
    convex_union.insert(
      convex_union.end(), interm_union.begin(), interm_union.end());
  }

  //check if the new system is different

  if(convex_union != this->sys.constraints)
  {
    std::cout << "the convex hull is:" << std::endl;
    print_cons(convex_union);
    is_modified = true;
    this->sys.constraints = convex_union;
  }

  return is_modified;
}

tvpi_systemt::dimensiont tvpi_domaint::lookup_binding(symbol_exprt symbol)
{
  tvpi_systemt::dimensiont result_dim = -1;
  std::__map_iterator mapping_it = binding.find(symbol);
  if(mapping_it != binding.end())
  {
    result_dim = binding[symbol];
  }
  return result_dim;
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
  const tvpi_domaint::binding_map &left,
  tvpi_domaint::binding_map &right,
  const tvpi_systemt &a,
  tvpi_systemt &b)
{
  for(const auto &binding_pair : left)
  {
    auto loc = right.find(binding_pair.first);
    if(loc != right.end())
    {
    }
    if(loc != right.end() && binding_pair.second != loc->second)
    {
      b.constraints = relabel_ineqs(b, loc->second, binding_pair.second);
      right[binding_pair.first] = binding_pair.second;
    }
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

std::vector<std::shared_ptr<inequality>>
filter(const tvpi_systemt &sys, std::vector<std::string> &target_vars)
{
  std::vector<std::shared_ptr<inequality>> res;
  for(const std::shared_ptr<inequality> &i : sys.constraints)
  {
    std::vector<std::string> found_vars = i->vars();
    //std::cout << "size vars: " << found_vars.size() << std::endl;

    if(found_vars.size() >= 1)
    {
      //std::cout << found_vars[0] << " " << found_vars[1] << std::endl;
      //std::cout << "target vars: " << target_vars.size() << std::endl;
      //std::cout << target_vars[0] << std::endl;

      if(found_vars == target_vars)
      {
        res.push_back(i);
      }
    }
  }
  return res;
}
