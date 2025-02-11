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
  out << "Number of dimensions: " << this->sys.dimension_counter << std::endl;
  out << "Binding of size: " << this->binding.size()
      << " contains: " << std::endl;
  for(const auto &[symbol, dimension] : binding)
    out << id2string(symbol.get_identifier()) << "-> " << dimension
        << std::endl;
  out << "number of cons: " << this->sys.constraints.size() << std::endl;
  for(auto i : this->sys.constraints)
    out << i->to_string() << std::endl;
}

//Create a dimension in the TVPI-system that over-approximates
//the value of an expression.
tvpi_systemt::dimensiont
tvpi_domaint::eval(exprt e)
{
  std::cerr << "in eval with e: " << e.pretty() << std::endl;

  if(e.id() == ID_constant)
  {
    tvpi_systemt::dimensiont c = this->sys.add_new_dimension();
    mp_integer const_e = numeric_cast_v<mp_integer>(to_constant_expr(e));
    this->sys.add_inequality(1, "d" + integer2string(c), 0, "d", const_e);
    this->sys.add_inequality(-1, "d" + integer2string(c), 0, "d", -const_e);
    return c;
  }
  else if(e.id() == ID_symbol)
  {
    std::cerr << "in ID:" << std::endl;
    std::cerr << e.pretty() << std::endl;
    tvpi_systemt::dimensiont tmp = this->sys.add_new_dimension();
    symbol_exprt symbol = to_symbol_expr(e);
    tvpi_systemt::dimensiont result = lookup_binding(symbol);
    if(result > 0)
    {
      std::cerr << "The variable assigment was found" << std::endl;
      references[symbol] = references[symbol] + 1;
      std::cout<<"The refrence count for this dimension is: "<<references[symbol]<<std::endl;
      tmp = result;
    }
    std::cerr << "A tmp was generated for the assignemnt" << tmp << std::endl;
    return tmp;
  }
  else if(e.id() == ID_plus)
  {
    tvpi_systemt::dimensiont sum_dim = this->sys.add_new_dimension();
    plus_exprt plus_e = to_plus_expr(e);

    if(plus_e.op0().is_constant()&&plus_e.op1().is_constant())
    {
      mp_integer const_left = numeric_cast_v<mp_integer>(to_constant_expr(plus_e.op0()));
      mp_integer const_right = numeric_cast_v<mp_integer>(to_constant_expr(plus_e.op1()));
      this->sys.add_inequality(1,"d"+integer2string(sum_dim),0,"d",const_left+const_right);
      this->sys.add_inequality(-1,"d"+integer2string(sum_dim),0,"d",-(const_left+const_right));
    }


    tvpi_systemt::dimensiont left =
      eval(to_symbol_expr(plus_e.op0()));
    tvpi_systemt::dimensiont right =
      eval(to_symbol_expr(plus_e.op1()));

    std::optional<mp_integer> u_bound_left = this->sys.get_ub(left);
    std::optional<mp_integer> l_bound_left = this->sys.get_lb(left);
    std::optional<mp_integer> u_bound_right = this->sys.get_ub(right);
    std::optional<mp_integer> l_bound_right = this->sys.get_lb(right);

    if(u_bound_left.has_value()&&u_bound_right.has_value()){


    auto filter_left = this->sys.tvpi_systemt::filter_ineqs(left);
    auto filter_right = this->sys.tvpi_systemt::filter_ineqs(right);

    this->sys.add_inequality(
        -1,
        "d" + integer2string(left),
        1,
        "d" + integer2string(sum_dim),
        u_bound_right.value());
    this->sys.add_inequality(
        -1,
        "d" + integer2string(right),
        1,
        "d" + integer2string(sum_dim),
        u_bound_left.value());


    }



    //to_constant_expr
    //tvpi_systemt::dimensiont left =
    //eval(to_symbol_expr(plus_e.op0()));

    /*

    tvpi_systemt::dimensiont left =
      eval(to_symbol_expr(plus_e.op0()));
    tvpi_systemt::dimensiont right =
      eval(to_symbol_expr(plus_e.op1()));

    std::cerr << "left: " << left << " right: " << right << std::endl;


    */
    /*
    //auto filter_left = this->sys.tvpi_systemt::filter_ineqs(left);
    //auto filter_right = this->sys.tvpi_systemt::filter_ineqs(right);

    //std::optional<std::shared_ptr<unary_inequality>> u_ineq_left = tvpi_systemt::cast_to_unary(filter_left[0]);
    //std::optional<std::shared_ptr<unary_inequality>> u_ineq_right = tvpi_systemt::cast_to_unary(filter_right[0]);

    //Theorem 1
    if(u_bound_left.has_value() && u_bound_right.has_value())
    {
      this->sys.add_inequality(
        -1,
        "d" + integer2string(left),
        1,
        "d" + integer2string(sum_dim),
        u_bound_right.value());
      this->sys.add_inequality(
        -1,
        "d" + integer2string(right),
        1,
        "d" + integer2string(sum_dim),
        u_bound_left.value());
    }
    //Theorem 2
    if(l_bound_left.has_value() && l_bound_right.has_value())
    {
      this->sys.add_inequality(
        1,
        "d" + integer2string(left),
        -1,
        "d" + integer2string(sum_dim),
        -u_bound_right.value());
      this->sys.add_inequality(
        1,
        "d" + integer2string(right),
        -1,
        "d" + integer2string(sum_dim),
        -u_bound_left.value());
    }
    //Theorem 3

    //Theorem 4
    //Theorem N

    */

    return sum_dim;
  }
  else if(e.id() == ID_minus)
  {
    binary_exprt bin_exp = to_binary_expr(e);
    return eval(
      plus_exprt(bin_exp.lhs(), unary_minus_exprt(bin_exp.rhs())));
  }
  else if(e.id() == ID_unary_minus)
  {
    tvpi_systemt::dimensiont u_min_dim = this->sys.add_new_dimension();
    unary_minus_exprt u_min_e = to_unary_minus_expr(e);
    tvpi_systemt::dimensiont dt = eval(u_min_e.op());
    mp_integer up = this->sys.get_ub(dt).value();
    mp_integer lb = this->sys.get_lb(dt).value();
    this->sys.add_inequality(-1, "d" + integer2string(dt), 0, "d", up);
    this->sys.add_inequality(1, "d" + integer2string(dt), 0, "d", lb);
    return dt;
  }
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
  }
  else
  {
    std::cerr << "No evaluation for e:" << id2string(e.id()) << std::endl;
    return -1;
  }

  UNREACHABLE;

  /*
  if(e.id() == ID_symbol)
  {
  std::cerr<<"We are reading IDs"<<std::endl;
// LOOK UP BINDING RETTURN VALUE 
  }
  else if(e.id() == ID_constant)
  {
    // RESTRICT TO INTEGERS
    // NEW TEMP DIMENSION
    // ADD TWO TVPI CONSTRAINTS (BOUNDS OF THAT CONSTANT)
  if(e.id() == ID_plus)
  {
    //new dimension for the sum
    //take care of left and right to
    //link the 
    //e.g. x+y
    //filter the the constraints that are involve the used dimensions x, y
    //get x , get y 
    // return the tmp "sum"
    // tests for this should land in regression 
    // 
  }
  }*/
}

// Takes a bool expression and reduces the domain
// to an overapproximation of when this condition holds
void tvpi_domaint::assume(exprt e)
{
  // Can only assume logical conditions
  std::cout<<"ID here: "<<e.id_string()<<std::endl;
  //PRECONDITION(e.id() == ID_bool);
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

    std::cout<<"label left"<<label_l<<" label right"<<label_r<<std::endl;

    //1 ∗a−1 ∗b ⩽ 0 ∧−1 ∗a + 1 ∗b ⩽ 0

    sys.add_inequality(1, label_l, -1, label_r, 0);
    sys.add_inequality(-1, label_l, 1, label_r, 0);
    return;
    }
  if(e.id() == ID_le)
  {
    //a<=b ->1*a - 1*b <= 0
    //auto &rel = to_binary_relation_expr(e);
    //tvpi_systemt::dimensiont l = eval(rel.lhs(), temporaries);
    //tvpi_systemt::dimensiont r = eval(rel.rhs(), temporaries);
    //std::string label_l, label_r;
    //label_l = "d" + integer2string(l);
    //label_r = "d" + integer2string(r);
    //this->sys.add_inequality(1, label_l, -1, label_r, 0);
  }
  if(e.id() == ID_lt){
    auto bin = to_binary_expr(e);
    tvpi_systemt::dimensiont l = eval(bin.lhs());
    tvpi_systemt::dimensiont r = eval(bin.rhs());
    this->sys.add_inequality(1, "d"+integer2string(l),  -1, "d"+integer2string(r), -1);
    return;
  }
  if(e.id() == ID_ge)
  {
    auto bin = to_binary_expr(e);
    tvpi_systemt::dimensiont l = eval(bin.lhs());
    tvpi_systemt::dimensiont r = eval(bin.rhs());
    this->sys.add_inequality(-1, "d"+integer2string(l),  1, "d"+integer2string(r), 0);
    return;
  }
  if(e.id() == ID_and)
  {
    //assume(to_and_expr(e).op0());
    //assume(to_and_expr(e).op1());
  }
  if(e.id() == ID_or)
  {
    // These requires some magic
    // Don't worry about it for now.
  }
  
  if(e.id() == ID_not)
  { 
    not_exprt tmp(to_not_expr(e));
    if(tmp.op().id()==ID_not){
      assume(to_not_expr(tmp.op()).op());
    }
    else if(tmp.op().id()==ID_lt){
      auto rel = to_binary_relation_expr(tmp.op());

      assume(greater_than_or_equal_exprt(rel.lhs(),rel.rhs()));
    }
    // These requires some magic
    // Don't worry about it for now.
  }
  

  
   std::cerr << "If only I knew how to assume a " << id2string(e.id())
              << std::endl;
  

  /*
for (const auto &t : temporaries) {
//sys.existential_projection(t);
}
*/
}

// Updates the binding of the symbol to point to a dimension
// that over-approximates e
void tvpi_domaint::assign(symbol_exprt lhs, exprt e)
{
  std::cerr << "We are in assign with: " << id2string(lhs.get_identifier()) << std::endl;
  tvpi_systemt::dimensiont ev = eval(e);

  //eval has an abstraction
  //std::cerr<<"Ev for "<<e.pretty()<<"is: "<<ev<<std::endl;

  if(ev >= 0)
  {
    if(lookup_binding(lhs) >= 0)
    {
      //old dimension
      tvpi_systemt::dimensiont tmp = this->binding[lhs];
      std::cerr << "On " << lhs.pretty() << "I found" << std::endl;
      this->binding[lhs] = ev;
      //this->sys.existential_project(tmp);
    }
    else
    {
      std::cerr << "Binding was not" << lhs.pretty()
                << "found, new binding pair was created" << std::endl;
      this->sys.add_new_dimension();
      this->binding.insert(std::make_pair(lhs, ev));
    }
  }

  //for(const mp_integer &t : temporaries)
  //{
  //this->sys.existential_project(t);
  //}
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
  std::cerr << "TVPI domain @ 0x" << this << " transform using instruction "
            << from->current_location()->location_number << '\n';

  const goto_programt::instructiont &instruction = *(from->current_location());
  std::cerr << "The instruction type is: " << instruction.to_string()
            << std::endl;

  // If e is an exprt (an expression) then
  //   std::cerr << e.pretty()
  // prints it out

  // Normally how the state of the domain is updated would depend on
  // what the instruction was (see below).  However the state we are
  // storing is so simple that we can just...

  switch(instruction.type())
  {
    /** These are the instructions you actually need to implement **/
  case DECL:
    //Add new dimension, binding
    this->sys.add_new_dimension();
    this->binding.insert(std::make_pair(
      to_code_decl(instruction.code()).symbol(), this->sys.dimension_counter));
    break;

  case DEAD:
    //Project out the var, remove from binding and decrease the dimension_counter
    //if(
    //  id2string(to_code_dead(instruction.code()).symbol().get_identifier())
    //    .find("return_value") == std::string::npos)
    //{
    //this->sys.existential_project(
    //  this->binding[to_code_dead(instruction.code()).symbol()]);
    //this->binding.erase(to_code_dead(instruction.code()).symbol());
    //this->sys.dimension_counter -= 1;
    //}
    break;

  case ASSIGN:
    if(
      id2string(to_symbol_expr(instruction.assign_lhs()).get_identifier())
        .find("__CPROVER") == std::string::npos)
    {
      assign(
        to_symbol_expr(instruction.assign_lhs()), instruction.assign_rhs());
    }
    break;

  case GOTO:
  {
    // Comparing iterators is safe as the target must be within the same list
    // of instructions because this is a GOTO.
    std::cout<<"inside GOTO"<<std::endl;
    locationt next = from->current_location();
    next++;
    if(
      from->current_location()->get_target() !=
      next) // If equal then effectively a SKIP
    {
      if(next == to->current_location())
      {
        // Branch is not taken
        std::cout<<"branch not taken"<<std::endl;
        assume(not_exprt(instruction.condition()));
      }
      else
      {
        // Branch is taken
         std::cout<<"branch taken"<<std::endl;
         assume(instruction.condition());
      }
    }
    break;
  }

  case FUNCTION_CALL:
  {
    // Function calls are a bit of a fiddle...
    // const code_function_callt &code_function_call =
    //   to_code_function_call(instruction.code);
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

  case ASSUME:
    // It is safe to over-approximate these by ... ignoring them!
    //ASSUME allows to restrict the
    //std::cerr<<"condition"<<instruction.condition().pretty()<<std::endl;
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

  return;
}

//trace_ptrt can be seen as location, will be used for widening
bool tvpi_domaint::merge(const tvpi_domaint &b, trace_ptrt, trace_ptrt)
{
  //std::cerr << "CASE 0: ENTRY" << std::endl;

  if(b.is_bottom())
  {
    return false;
    std::cerr << "CASE 1: B IS BOTTOM" << std::endl;
  }

  if(this->is_bottom())
  {
    INVARIANT(!b.is_bottom(), "CASE HANDLED");
    // copy
    std::cerr << "a size: " << this->sys.constraints.size()
              << " b size: " << b.sys.constraints.size() << std::endl;
    this->sys.constraints = b.sys.constraints;
    this->binding = b.binding;
    this->sys.dimension_counter = b.sys.dimension_counter;
    std::cerr << "a: " << this->sys.dimension_counter
              << " b: " << b.sys.dimension_counter << std::endl;
    std::cerr << "CASE 2: A is BOTTOM" << std::endl;
    return true;
  }

  INVARIANT(!this->is_bottom() && !b.is_bottom(), "Case handled");

  bool is_modified = false;
  // handle top
  if(b.is_top())
  {
    // change if it was not top
    is_modified = !this->is_top();

    make_top();
    std::cerr << "CASE 3: B IS TOP" << std::endl;
    return is_modified;
  }

  //CASE HULL ENTRY
  std::cerr << "CASE 4: CALL TO CONVEX UNION" << std::endl;

  binding_map copy_left = this->binding;
  binding_map copy_right = b.binding;

  tvpi_systemt copy_a = this->sys;
  tvpi_systemt copy_b = b.sys;

  align_bindings(copy_left,copy_right,copy_a,copy_b);
  std::set<std::string> existing_relations = find_relations(copy_a,copy_b);

  //normalise so the hull works 
  //copy_a.constraints = add_var(copy_a);
  //copy_b.constraints = add_var(copy_b);

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

    std::cout << first_var << " part2: " << second_var << std::endl;
    if(first_var != second_var)
    {
      std::cout<<"first var: "<<first_var<<" second var: "<<second_var<<std::endl;
      target_vars = {first_var, second_var};
    }
    else
    {
      target_vars = {first_var};
    }
    
   std::cout<<"vars here::"<<std::endl;
  print_ineq(copy_a.constraints[0]);


    filter_left = filter(copy_a, target_vars);
    filter_right = filter(copy_b, target_vars);
    std::cout << "left filter dy" << std::endl;
    print_cons(filter_left);
    std::cout << "right filter dy" << std::endl;
    print_cons(filter_right);
    extract_dimensions(filter_left);
    extract_dimensions(filter_right);
    interm_union = join::calc_hull(filter_left, filter_right);
    convex_union.insert(convex_union.end(), interm_union.begin(), interm_union.end());
  }

  //check if the new system is different

  if(convex_union != this->sys.constraints)
  {
    std::cout<<"the convex hull is:"<<std::endl;
    print_cons(convex_union);
    is_modified = true;
    this->sys.constraints = convex_union;
    return is_modified;
  }

  return is_modified;
}

tvpi_systemt::dimensiont tvpi_domaint::lookup_binding(symbol_exprt sym)
{
  tvpi_systemt::dimensiont result_dim = -1;
  std::__map_iterator mapping_it = binding.find(sym);
  if(mapping_it != binding.end())
  {
    result_dim = binding[sym];
  }
  std::cerr << "result dim is: " << result_dim << std::endl;
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
      std::cout<<"size vars: "<<found_vars.size()<<std::endl;
      std::cout<<found_vars[0]<<" "<<found_vars[1]<<std::endl;
      std::cout<<"target vars: "<<target_vars.size()<<std::endl;
      std::cout<<target_vars[0]<<std::endl;

      if(found_vars == target_vars)
      {
        res.push_back(i);
      }
    
  }
  return res;
}

std::vector<std::shared_ptr<inequality>> add_var(tvpi_systemt sys)
{
  std::vector<std::shared_ptr<inequality>> res;
  for(auto &c : sys.constraints)
  {
    if(cast_to_unary(c))
    {
      std::cout << "can cast this" << std::endl;
      auto i = cast_to_unary(c);
      std::shared_ptr<dyadic_inequality> d =
        std::make_shared<dyadic_inequality>(i->x, "", i->a, 0, i->c);
      res.push_back(d);
    }
    else
    {
      res.push_back(c);
    }
  }
  return res;
}