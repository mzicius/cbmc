
#include "tvpi_systemt.h"
#include "inequality_factory.h"
#include <algorithm>
#include <fstream>

tvpi_systemt::tvpi_systemt()
{
  std::cerr << "Initialize TVPI system" << std::endl;
  constraints = {};
  dimension_counter = -1;
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
  dimensiont new_dim = ++dimension_counter;
  references[new_dim] = 0;
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
tvpi_systemt::filter(const std::vector<std::string> &target_vars)
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

std::optional<mp_integer> tvpi_systemt::get_ub(mp_integer dimensiont)
{
  std::vector<std::shared_ptr<inequality>> ineqs =
  this->filter({"d"+integer2string(dimensiont)});

  std::vector<std::shared_ptr<unary_inequality>> unary_ineqs;

  for(auto c:ineqs){

    auto new_in = cast_to_unary(c);

    unary_ineqs.push_back(new_in);
  }



  if(!unary_ineqs.empty())
  {
    mp_integer u_bound;

    if(unary_ineqs.size() > 1)
    {
      mp_integer a = unary_ineqs[0]->a * unary_ineqs[0]->c;
      mp_integer b = unary_ineqs[1]->a * unary_ineqs[1]->c;
      u_bound = (a > b) ? a : b;
    }
    else
    {
      u_bound = unary_ineqs[0]->a * unary_ineqs[0]->c;
    }

    return u_bound;
  }

  return std::nullopt;
}

std::optional<mp_integer> tvpi_systemt::get_lb(mp_integer dimensiont)
{
  std::vector<std::shared_ptr<inequality>> ineqs =
    this->filter({"d"+integer2string(dimensiont)});

    std::vector<std::shared_ptr<unary_inequality>> unary_ineqs;

    for(auto c:ineqs){

      auto new_in = cast_to_unary(c);
  
      unary_ineqs.push_back(new_in);
    }
  
  


  if(!unary_ineqs.empty())
  {
    mp_integer l_bound;

    if(unary_ineqs.size() > 1)
    {
      mp_integer a = unary_ineqs[0]->a * unary_ineqs[0]->c;
      mp_integer b = unary_ineqs[1]->a * unary_ineqs[1]->c;
      l_bound = (a < b) ? a : b;
    }
    else
    {
      l_bound = unary_ineqs[0]->a * unary_ineqs[0]->c;
    }

    return l_bound;
  }

  return std::nullopt;
}
