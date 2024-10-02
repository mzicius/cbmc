#include "tvpi_domaint.h"

#include <util/mp_arith.h>

#include "complete.h"
#include "join.h"
#include "smt_printer.h"
#include "tikz_printer.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>

tvpi_domaint::tvpi_domaint()
{
  // This must set the domain to bottom i.e. "no possible values"
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
  tvpi_systemt();
}

void tvpi_domaint::make_entry()
{
  // This is fine for almost everyone
  dimension_counter = 0;
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
  return sys.constraints.empty();
}

void tvpi_domaint::output(
  std::ostream &out,
  const ai_baset &ai,
  const namespacet &ns) const
{
  //print binding
  out << "binding size: " << binding.size();
  //TVPI constraints
}

//Create a dimension in the TVPI-system that over-approximates
//the value of an expression.
tvpi_systemt::dimensiont eval(exprt e, std::vector<tvpi_systemt::dimensiont> &temporaries)
{
  if(e.type().id()== ID_equal)
  {

  }
  if(e.type().id() == ID_symbol)
  {

  }
  else if(e.type().id() == ID_constant)
  {
  }
  else if(e.type().id() == ID_plus)
  {
  }
  else
  {
    std::cerr << "If only I knew how to eval a " << id2string(e.id())
              << std::endl;
  }

  UNREACHABLE;

  //special dimension value
  return -1;
}

// Takes a boolean expression and reduces the domain
// to an overapproximation of when this condition holds
void tvpi_domaint::assume(exprt e)
{
  // Can only assume logical conditions
  PRECONDITION(e.type().id() == ID_bool);

  // May need to add some extra dimensions..
  std::vector<tvpi_systemt::dimensiont> temporaries;

  if(e.type().id() == ID_equal)
  {
    //turn to object and then recurse
    //cant link inequalities due to l and r being numbers and not strings
    //how can the map be examinded for keys what if two keys (vars) hold the same dimensions
    tvpi_systemt::dimensiont l = eval(to_index_expr(e).op0(), temporaries);
    tvpi_systemt::dimensiont r = eval(to_index_expr(e).op1(), temporaries);

    std::string label_l, label_r;
    label_l  =  "d" + integer2string(l);
    label_r  = "d" + integer2string(r);

    sys.add_inequality( 1, label_l, -1, label_r, 0);
    sys.add_inequality(-1, label_l,  1, label_r, 0);
  }
  if(e.type().id() == ID_le)
  {
    // tvpi_systemt::dimensiont l = eval(less_equal_zero, temporaries);
    //tvpi_domaint::dimensiont r = eval(e.right(), temporaries);
    // Link the two!
    //s.add_inequality( 1, l, -1, r, 0);
  }
  if(e.type().id() == ID_ge)
  {
    //tvpi_domaint::dimensiont l = eval(e.left(), temporaries);
    //tvpi_domaint::dimensiont r = eval(e.right(), temporaries);

    // Link the two!
    //s.add_inequality(-1, l,  1, r, 0);
  }

  if(e.type().id() == ID_and)
  {
    assume(to_and_expr(e).op0());
    assume(to_and_expr(e).op1());
  }
  if(e.type().id() == ID_or)
  {
    // These requires some magic
    // Don't worry about it for now.
  }
  if(e.type().id() == ID_not)
  {
    // These requires some magic
    // Don't worry about it for now.
  }
  else
  {
    std::cerr << "If only I knew how to assume a " << id2string(e.id())
              << std::endl;
  }

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
  std::vector<tvpi_systemt::dimensiont> temporaries;
  tvpi_systemt::dimensiont ev = eval(e, temporaries);

  //search for mapping
  auto mapping_it = binding.find(lhs);
  if(mapping_it != binding.end())
  {
  
    binding.insert(std::make_pair(lhs, ev));
    tvpi_systemt::dimensiont tmp = binding[lhs];
    sys.existential_project(tmp);
  }
  else
  {
    binding.insert(std::make_pair(lhs, ev));
  }

  for(const mp_integer &t : temporaries)
  {
    sys.existential_project(t);
  }
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

  //
  //std::cerr<<"size of labels: "<<labels.size()<<std::endl;

  // If e is an exprt (an expression) then
  //   std::cerr << e.pretty()
  // prints it out

  // Normally how the state of the domain is updated would depend on
  // what the instruction was (see below).  However the state we are
  // storing is so simple that we can just...

  const goto_programt::instructiont &instruction = *(from->current_location());

  switch(instruction.type())
  {
    /** These are the instructions you actually need to implement **/
  case DECL:
    //Create new dimension and add to binding
    this->sys.add_new_dimension();
    this->binding.insert(std::make_pair(
    to_code_decl(instruction.code()).symbol(), dimension_counter - 1));
    break;

  case DEAD:
    //Remove variable from binding and decrease the dimension_counter
    //existential project
    this->sys.existential_project(this->binding[to_code_dead(instruction.code()).symbol()]);
    this->binding.erase(to_code_dead(instruction.code()).symbol());
    dimension_counter -= 1;
    break;

  case ASSIGN:
    assign(
      to_symbol_expr(to_code_assign(instruction.code()).lhs()),
      to_code_assign(instruction.code()).rhs());
    break;

  case GOTO:
  {
    // Comparing iterators is safe as the target must be within the same list
    // of instructions because this is a GOTO.
    locationt next = from->current_location();
    next++;
    if(
      from->current_location()->get_target() !=
      next) // If equal then effectively a SKIP
    {
      if(next == to->current_location())
      {
        // Branch is not taken
      }
      else
      {
        // Branch is taken
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
    assume(instruction.code());
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
  std::cerr << "CASE 0: ENTRY" << std::endl;

  if(b.is_bottom())
  {
    return false;
    std::cerr << "CASE 1: B IS BOTTOM" << std::endl;
  }

  if(this->is_bottom())
  {
    INVARIANT(!b.is_bottom(), "CASE HANDLED");
    // copy
    this->sys.constraints = b.sys.constraints;
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

  std::vector<std::shared_ptr<inequality>> new_sys =
    join::calc_hull(this->sys.constraints, b.sys.constraints);

  //check if the new system is different

  if(new_sys != this->sys.constraints)
  {
    is_modified = true;
    this->sys.constraints = new_sys;
    std::cerr << "CASE 4: CALL TO CONVEX UNION" << std::endl;
    return is_modified;
  }

  return is_modified;
}
