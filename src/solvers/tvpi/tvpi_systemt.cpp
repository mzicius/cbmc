
#include "tvpi_systemt.h"

#include "inequality_factory.h"

#include <algorithm>
#include <fstream>

tvpi_systemt::dimensiont tvpi_systemt::dim_counter = -1;

tvpi_systemt::dimensiont tvpi_systemt::get_current_dim() const
{
  return dim_counter;
}

tvpi_systemt::tvpi_systemt()
{
  std::cerr << "Initialize TVPI system" << std::endl;
  constraints = {};
}

void tvpi_systemt::existential_project(mp_integer dimensiont)
{
  std::string var = "d" + integer2string(dimensiont);
  std::cerr << "projecting out: " << var << std::endl;
  std::vector<std::shared_ptr<inequality>> project;
  for(std::shared_ptr<inequality> c : constraints)
  {
    std::vector<std::string> contents = c->vars();
    auto loc = find(contents.begin(), contents.end(), var);
    if(loc == contents.end())
    {
      project.push_back(c);
    }
  }
  constraints = project;
}

void tvpi_systemt::make_unsat_system()
{
  constraints = {std::make_shared<constant_inequality>(false)};
}

tvpi_systemt::dimensiont tvpi_systemt::add_new_dimension()
{
  dimensiont new_dim = ++dim_counter;
  return new_dim;
}

void tvpi_systemt::add_inequality(
  mp_integer a,
  std::string x,
  mp_integer b,
  std::string y,
  mp_integer c)
{
  auto i = inequality_factory::make_inequality(x, y, a, b, c);
  constraints.push_back(i);
  std::cerr << "new_ineq: " << i->to_string() << std::endl;
  constraints = complete::closure(constraints);
}

void tvpi_systemt::add_inequality(
  rationalt a,
  std::string x,
  rationalt b,
  std::string y,
  rationalt c)
{
  auto i = inequality_factory::make_inequality(x, y, a, b, c);
  constraints.push_back(i);
  std::cerr << "new_ineq: " << i->to_string() << std::endl;
  constraints = complete::closure(constraints);
}

void tvpi_systemt::print_system()
{
  std::ofstream system_trace;
  system_trace.open("../../logs/system_trace.txt");
  for(const std::shared_ptr<inequality> &i : constraints)
  {
    system_trace << i->to_string() << "\n";
  }
  system_trace.close();
}

std::vector<std::shared_ptr<inequality>>
tvpi_systemt::filter(const std::vector<std::string> &target_vars) const
{
  std::vector<std::shared_ptr<inequality>> result;
  for(const std::shared_ptr<inequality> &c : constraints)
  {
    if(c->vars() == target_vars)
    {
      result.push_back(c);
    }
  }
  return result;
}

//x<=3
//upper bound is when the coeff is positive and the sign is <=
std::optional<rationalt> tvpi_systemt::get_ub(mp_integer dimensiont)
{
  std::vector<std::shared_ptr<inequality>> ineqs =
    this->filter({"d" + integer2string(dimensiont)});

  std::vector<std::shared_ptr<unary_inequality>> unary_ineqs;

  for(auto c : ineqs)
  {
    std::shared_ptr<unary_inequality> new_in = cast_to_unary(c);

    unary_ineqs.push_back(new_in);
  }

  if(!unary_ineqs.empty())
  {
    rationalt u_bound;

    mp_integer a1, c1;
    std::shared_ptr<unary_inequality> u1 = unary_ineqs[0];
    a1 = u1->a;
    c1 = u1->c;

    if((a1 > 0 && c1 > 0) || (a1 > 0 && c1 < 0))
    {
      u_bound = rationalt(c1) / rationalt(a1);
    }

    if(unary_ineqs.size() > 1)
    {
      mp_integer a2, c2;
      std::shared_ptr<unary_inequality> u2 = unary_ineqs[1];
      a2 = u2->a;
      c2 = u2->c;

      if((a2 > 0 && c2 > 0) || (a2 > 0 && c2 < 0))
      {
        u_bound = rationalt(c2) / rationalt(a2);
      }
    }

    return u_bound;
  }

  return std::nullopt;
}

//-x<=3
//-x<=-3
//lower bound is when the coeff is negative or the sign is >=
std::optional<rationalt> tvpi_systemt::get_lb(mp_integer dimensiont)
{
  std::vector<std::shared_ptr<inequality>> ineqs =
    this->filter({"d" + integer2string(dimensiont)});

  std::vector<std::shared_ptr<unary_inequality>> unary_ineqs;

  for(auto c : ineqs)
  {
    std::shared_ptr<unary_inequality> new_in = cast_to_unary(c);

    unary_ineqs.push_back(new_in);
  }

  if(!unary_ineqs.empty())
  {
    rationalt l_bound;

    mp_integer a1, c1;
    std::shared_ptr<unary_inequality> u1 = unary_ineqs[0];
    a1 = u1->a;
    c1 = u1->c;

    if((a1 < 0 && c1 > 0) || (a1 < 0 && c1 < 0))
    {
      l_bound = rationalt(c1) / rationalt(a1);
    }

    if(unary_ineqs.size() > 1)
    {
      mp_integer a2, c2;
      std::shared_ptr<unary_inequality> u2 = unary_ineqs[1];
      a2 = u2->a;
      c2 = u2->c;

      if((a2 < 0 && c2 > 0) || (a2 < 0 && c2 < 0))
      {
        l_bound = rationalt(c2) / rationalt(a2);
      }
    }

    return l_bound;
  }

  return std::nullopt;
}

std::vector<std::shared_ptr<inequality>>
tvpi_systemt::intersect(const std::vector<std::shared_ptr<inequality>> &b)
{
  std::vector<std::shared_ptr<inequality>> intersection;

  for(auto con_a : constraints)
  {
    for(auto con_b : b)
    {
      if(con_a == con_b)
      {
        intersection.push_back(con_a);
      }
    }
  }

  return intersection;
}

bool tvpi_systemt::is_equal(const std::vector<std::shared_ptr<inequality>> &b)
{
  //check if sizes are dif
  //check
  if(constraints.size() != b.size())
  {
    return false;
  }
  else
  {
    return std::equal(b.begin(), b.end(), constraints.begin());
  }
}

std::vector<std::shared_ptr<inequality>> tvpi_systemt::to_canon()
{
  //Part I
  //constraints are from a closed system
  std::vector<std::shared_ptr<inequality>> output = constraints;

  //Part II
  //output of canon is equiv to the input in every projection of that system

  //to achieve projections we need the variable relations in the system
  std::set<std::string> relations;
  //relations = find_relations(constraints,{});
  //slice up the systems and go back to them together

  //Case 3 says that only the tightest bounds on single variables are represented.
  //redundancy check

  //Case 4 says that the output is minimal in the sense that if a non-unary constraint is
  //removed it will change the space represented.

  return output;
}