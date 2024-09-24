#include "tvpi_systemt.h"

#include <util/mp_arith.h>

#include "inequality_factory.h"
#include "tvpi_domaint.h"

tvpi_systemt::tvpi_systemt()
{
  constraints = {};
}

void tvpi_systemt::existential_project(std::string var)
{
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
  return 0;
}

std::shared_ptr<inequality> tvpi_systemt::add_inequality(
  mp_integer a,
  std::string x,
  mp_integer b,
  std::string y,
  mp_integer c)
{
  return inequality_factory::make_inequality(x, y, a, b, c);
}