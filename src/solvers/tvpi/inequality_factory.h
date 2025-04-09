#ifndef INEQUALITY_FACTORY_H
#define INEQUALITY_FACTORY_H

#include "constant_inequality.h"
#include "inequality.h"
#include "unary_inequality.h"

class inequality_factory
{
public:
  static std::shared_ptr<inequality> make_inequality(
    std::string x,
    std::string y,
    mp_integer a,
    mp_integer b,
    mp_integer c);

  static std::shared_ptr<inequality>
  make_inequality(std::string x, mp_integer a, mp_integer c);

  static std::shared_ptr<inequality> make_inequality(mp_integer c);

  static std::shared_ptr<inequality> make_inequality(
    std::string x,
    std::string y,
    rationalt a,
    rationalt b,
    rationalt c);

  static std::shared_ptr<inequality>
  make_inequality(std::string x, rationalt a, rationalt c);
};

#endif