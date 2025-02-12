#include <util/mp_arith.h>

#include <analyses/tvpi/tvpi_domaint.h>

#include "inequality_factory.h"

#include <algorithm>

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

//if the TVPI domaint hold the binding why is this function inside the TVPI system class
tvpi_systemt::dimensiont tvpi_systemt::add_new_dimension()
{
  return ++dimension_counter;
}

std::vector<std::shared_ptr<inequality>>
tvpi_systemt::filter_ineqs(mp_integer dimensiont)
{
  std::string var = "d" + integer2string(dimensiont);
  std::cerr << "filtering for: " << var << std::endl;
  std::vector<std::shared_ptr<inequality>> filtered;
  for(std::shared_ptr<inequality> c : constraints)
  {
    std::vector<std::string> contents = c->vars();
    auto loc = find(contents.begin(), contents.end(), var);
    if(loc != contents.end())
    {
      filtered.push_back(c);
    }
  }
  std::cerr << "Size of filter is: " << filtered.size() << std::endl;
  return filtered;
}

std::vector<std::shared_ptr<inequality>> tvpi_systemt::filter_unary(std::vector<std::string> vars){
  std::vector<std::shared_ptr<inequality>> filtered;
  for(std::shared_ptr<inequality> c : constraints)
  {
    if(c->arity()==1){
    std::vector<std::string> contents = c->vars();
    auto loc = find(contents.begin(), contents.end(),vars[0]);
    if(loc != contents.end())
    {
      filtered.push_back(c);
    }
    }
  }

  return filtered;
}
std::vector<std::shared_ptr<inequality>> tvpi_systemt::filter_dyadic(std::vector<std::string> vars){
  std::vector<std::shared_ptr<inequality>> filtered;
    for(std::shared_ptr<inequality> c : constraints)
  {
    if(c->arity()==2){
    std::vector<std::string> contents = c->vars();
    auto loc = find(contents.begin(), contents.end(), vars[0]);
    auto lc = find(contents.begin(), contents.end(), vars[1]);
    if(loc != contents.end() && lc != contents.end())
    {
      filtered.push_back(c);
    }
  }
  }

  return filtered;
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
  constraints = complete::closure(constraints);
}

std::optional<mp_integer> tvpi_systemt::get_ub(mp_integer dimensiont)
{
  std::vector<std::shared_ptr<inequality>> all_ineqs = filter_ineqs(dimensiont);
  std::vector<std::shared_ptr<unary_inequality>> unary_ineqs;
  for(std::shared_ptr<inequality> i : all_ineqs)
  {
    if(std::dynamic_pointer_cast<unary_inequality>(i) != nullptr)
    {
      std::shared_ptr<unary_inequality> u =
        std::dynamic_pointer_cast<unary_inequality>(i);
      unary_ineqs.push_back(u);
    }
  }

  if(!unary_ineqs.empty())
  {
    mp_integer u_bound;

    if(unary_ineqs.size() > 1)
    {
      mp_integer a = unary_ineqs[0]->c;
      mp_integer b = unary_ineqs[1]->c;
      u_bound = (a > b) ? a : b;
    }
    else
    {
      u_bound = unary_ineqs[0]->c;
    }

    return u_bound;
  }

  return std::nullopt;
}

std::optional<mp_integer> tvpi_systemt::get_lb(mp_integer dimensiont)
{
  std::vector<std::shared_ptr<inequality>> all_ineqs = filter_ineqs(dimensiont);
  std::vector<std::shared_ptr<unary_inequality>> unary_ineqs;
  for(std::shared_ptr<inequality> i : all_ineqs)
  {
    if(std::dynamic_pointer_cast<unary_inequality>(i) != nullptr)
    {
      std::shared_ptr<unary_inequality> u =
        std::dynamic_pointer_cast<unary_inequality>(i);
      unary_ineqs.push_back(u);
    }
  }

  if(!unary_ineqs.empty())
  {
    mp_integer l_bound;

    if(unary_ineqs.size() > 1)
    {
      mp_integer a = unary_ineqs[0]->c;
      mp_integer b = unary_ineqs[1]->c;
      l_bound = (a < b) ? a : b;
    }
    else
    {
      l_bound = unary_ineqs[0]->c;
    }

    return l_bound;
  }
  return std::nullopt;
}

std::vector<std::shared_ptr<inequality>>
tvpi_systemt::relabel(mp_integer old_d, mp_integer new_d)
{
  std::vector<std::shared_ptr<inequality>> old_labels = filter_ineqs(old_d);
  std::vector<std::shared_ptr<inequality>> new_labels;

  for(auto i : old_labels)
  {
    if(cast_to_unary(i))
    {
      auto u = cast_to_unary(i);
      new_labels.push_back(inequality_factory::make_inequality(
        "d" + integer2string(new_d), u->a, u->c));
    }
    else if(cast_to_dyadic(i))
    {
      auto d = cast_to_dyadic(i);
      std::shared_ptr<inequality> new_ineq;
      if(d->x == "d" + integer2string(old_d))
      {
        new_ineq = inequality_factory::make_inequality(
          "d" + integer2string(new_d), d->y, d->a, d->b, d->c);
      }
      else
      {
        new_ineq = inequality_factory::make_inequality(
          d->x, "d" + integer2string(new_d), d->a, d->b, d->c);
      }
      new_labels.push_back(new_ineq);
    }
  }

  return new_labels;
}