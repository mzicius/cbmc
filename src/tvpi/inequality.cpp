#include "inequality.h"

#include <util/invariant.h>

#include "constant_inequality.h"
#include "join.h"
#include "tikz_printer.h"
#include "unary_inequality.h"

#include <algorithm>
#include <iostream>
void print_vars(const std::vector<std::string> &sts)
{
  if(sts.empty())
    std::cout << "[]";
  else
  {
    for(std::string s : sts)
      std::cout << s + " ";
  }
  std::cout << std::endl;
}

void print_ins(const std::vector<std::shared_ptr<inequality>> ins)
{
  if(ins.empty())
    std::cout << "[]";
  else
  {
    for(auto i : ins)
      std::cout << i->to_string() + " ";
  }
  std::cout << std::endl;
}

/// \param i1
/// \return Variables of the inequality
std::vector<std::string> inequality::vars(std::shared_ptr<inequality> i1)
{
  auto v = vars();
  std::set<std::string> set(v.begin(), v.end());
  auto i1v = i1->vars();
  set.insert(i1v.begin(), i1v.end());
  return std::vector<std::string>(set.begin(), set.end());
}

/// \param i1
/// \param i2
/// \return Variables of the inequalities
std::vector<std::string>
inequality::vars(std::shared_ptr<inequality> i1, std::shared_ptr<inequality> i2)
{
  auto v = vars();
  std::set<std::string> set(v.begin(), v.end());
  auto i1v = i1->vars();
  set.insert(i1v.begin(), i1v.end());
  auto i2v = i2->vars();
  set.insert(i2v.begin(), i2v.end());
  return std::vector<std::string>(set.begin(), set.end());
}

/// \return Number of variables in the inequalities
int inequality::arity()
{
  return vars().size();
}

bool inequality::planar_entailed(std::shared_ptr<inequality> ineq1)
{
  std::vector<std::string> vars_ineq1 = vars(ineq1);
  std::vector<std::string> vars(vars_ineq1.begin(), vars_ineq1.end());

  std::shared_ptr<inequality> i = widen(vars);
  std::shared_ptr<inequality> i1 = ineq1->widen(vars);

  if(
    std::dynamic_pointer_cast<unary_inequality>(i) != nullptr &&
    std::dynamic_pointer_cast<unary_inequality>(i1) != nullptr)
  {
    std::shared_ptr<unary_inequality> u =
      std::dynamic_pointer_cast<unary_inequality>(i);
    std::shared_ptr<unary_inequality> u1 =
      std::dynamic_pointer_cast<unary_inequality>(i1);
    return planar_entailed(false, u->a, 0, u->c, u1->a, 0, u1->c);
  }
  else
  {
    std::shared_ptr<dyadic_inequality> d =
      std::dynamic_pointer_cast<dyadic_inequality>(i);
    std::shared_ptr<dyadic_inequality> d1 =
      std::dynamic_pointer_cast<dyadic_inequality>(i1);
    return planar_entailed(false, d->a, d->b, d->c, d1->a, d1->b, d1->c);
  }
}

bool inequality::planar_entailed(
  std::shared_ptr<inequality> ineq1,
  std::shared_ptr<inequality> ineq2)
{
  if(debugging)
    std::cout << "Inside entailed i1 i2" << std::endl;

  std::vector<std::string> vars_ineq1_2 = vars(ineq1, ineq2);
  std::vector<std::string> vars(vars_ineq1_2.begin(), vars_ineq1_2.end());

  if(debugging)
  {
    std::cout << "vars: ";
    print_vars(vars);
  }

  std::shared_ptr<inequality> i = widen(vars);
  std::shared_ptr<inequality> i1 = ineq1->widen(vars);
  std::shared_ptr<inequality> i2 = ineq2->widen(vars);

  if(debugging)
  {
    std::cout << "i: " << i->to_string() << std::endl;
    std::cout << "i1: " << i1->to_string() << std::endl;
    std::cout << "i2: " << i2->to_string() << std::endl;
  }

  if(
    std::dynamic_pointer_cast<unary_inequality>(i) != nullptr &&
    std::dynamic_pointer_cast<unary_inequality>(i1) != nullptr &&
    std::dynamic_pointer_cast<unary_inequality>(i2) != nullptr)
  {
    if(debugging)
      std::cout << "unary clause" << std::endl;
    std::shared_ptr<unary_inequality> u =
      std::dynamic_pointer_cast<unary_inequality>(i);
    std::shared_ptr<unary_inequality> u1 =
      std::dynamic_pointer_cast<unary_inequality>(i1);
    std::shared_ptr<unary_inequality> u2 =
      std::dynamic_pointer_cast<unary_inequality>(i2);

    //std::cout<<u1->a<<" "<<u1->c<<" "<<u2->a<<" "<<u2->c<<std::endl;

    bool inconsistent =
      planar_entailed(true, -u1->a, 0, -u1->c, u2->a, 0, u2->c);
    //std::cout<<u1->a<<std::endl;
    //System.out.println("inconsistent: " + ineq1 + ", " + ineq2 + " [" + inconsistent + "]");
    //xstd::cout<<"Result of inconsistent: "<<inconsistent<<std::endl;
    //auto decision = planar_entailed(false, u->a, 0, u->c, u1->a, 0, u1->c, u2->a, 0, u2->c);
    //std::cout<<"Result of private long entailment : "<<decision<<std::endl;

    return inconsistent ||
           planar_entailed(
             false, u->a, 0, u->c, u1->a, 0, u1->c, u2->a, 0, u2->c);
  }
  else
  {
    if(debugging)
      std::cout << "dyadic clause" << std::endl;
    std::shared_ptr<dyadic_inequality> d =
      std::dynamic_pointer_cast<dyadic_inequality>(i);
    std::shared_ptr<dyadic_inequality> d1 =
      std::dynamic_pointer_cast<dyadic_inequality>(i1);
    std::shared_ptr<dyadic_inequality> d2 =
      std::dynamic_pointer_cast<dyadic_inequality>(i2);

    bool inconsistent =
      planar_entailed(true, -d1->a, -d1->b, -d1->c, d2->a, d2->b, d2->c);
    //System.out.println("inconsistent: " + ineq1 + ", " + ineq2 + " [" + inconsistent + "]");
    //std::cout<<"Result of inconsistent: "<<inconsistent<<std::endl;
    return inconsistent ||
           planar_entailed(
             false, d->a, d->b, d->c, d1->a, d1->b, d1->c, d2->a, d2->b, d2->c);
  }
}

bool inequality::planar_entailed(
  bool strict,
  mp_integer a,
  mp_integer b,
  mp_integer c,
  mp_integer a1,
  mp_integer b1,
  mp_integer c1)
{
  if(debugging)
    std::cout << "Inside private short entailed" << std::endl;

  mp_integer d = (a1 * b) - (a * b1);

  //std::cout<<"d: "<<d<<std::endl;

  int sign_a = sign(a);
  int sign_b = sign(b);
  int sign_a1 = sign(a1);
  int sign_b1 = sign(b1);

  if(debugging)
    std::cout << a << " " << b << " " << c << " " << a1 << " " << b1 << " "
              << c1 << std::endl;
  //std::cout<<sign_a<<" "<<sign_b<<" "<<sign_a1<<" "<<sign_b1<<std::endl;

  if(sign(d) != 0)
  {
    return false;
  }
  else if(sign_a * sign_a1 < 0)
  {
    return false;
  }
  else if(sign_b * sign_b1 < 0)
  {
    return false;
  }
  else if(sign_a > 0)
  {
    if(strict)
      return strictly_less(a * c1, a1 * c);
    return less_equal(a * c1, a1 * c);
  }
  else if(sign_a < 0)
  {
    if(strict)
      return strictly_greater(a * c1, a1 * c);
    return greater_equal(a * c1, a1 * c);
  }
  else if(sign_b > 0)
  {
    if(strict)
      return strictly_less(b * c1, b1 * c);
    return less_equal(b * c1, b1 * c);
  }
  else if(sign_b < 0)
  {
    if(strict)
      return strictly_greater(b * c1, b1 * c);
    return greater_equal(b * c1, b1 * c);
  }
  else
  {
    return false;
  }
}

bool inequality::planar_entailed(
  bool strict,
  mp_integer a,
  mp_integer b,
  mp_integer c,
  mp_integer a1,
  mp_integer b1,
  mp_integer c1,
  mp_integer a2,
  mp_integer b2,
  mp_integer c2)
{
  mp_integer d = (a1 * b2) - (a2 * b1);
  int sign_d = sign(d);

  if(debugging)
  {
    std::cout << "inside planar long entailment:" << std::endl;
    std::cout << "sign_d: " << sign_d << std::endl;
    std::cout << a << " " << b << " " << c << " " << a1 << " " << b1 << " "
              << c1 << " " << a2 << " " << b2 << " " << c2 << std::endl;
  }

  if(sign_d == 0)
  {
    //std::cout<<planar_entailed(strict, a, b, c, a1, b1, c1)<<std::endl;
    //std::cout<<planar_entailed(strict, a, b, c, a2, b2, c2)<<std::endl;
    return planar_entailed(strict, a, b, c, a1, b1, c1) ||
           planar_entailed(strict, a, b, c, a2, b2, c2);
  }

  else
  {
    mp_integer d_lambda1 = (a * b2) - (a2 * b);
    mp_integer d_lambda2 = (a1 * b) - (a * b1);

    if(debugging)
      std::cout << "d_lambda1: " << d_lambda1 << " d_lambda2: " << d_lambda2
                << std::endl;

    if(sign_d > 0)
    {
      if(sign(d_lambda1) < 0 || sign(d_lambda2) < 0)
      {
        return false;
      }
      else
      {
        mp_integer c_prime = (d_lambda1 * c1) + (d_lambda2 * c2);
        if(strict)
          return strictly_less(c_prime, c * d);
        else
          return less_equal(c_prime, c * d);
      }
    }
    else
    {
      if(sign(d_lambda1) > 0 || sign(d_lambda2) > 0)
      {
        return false;
      }
      else
      {
        mp_integer c_prime = (d_lambda1 * c1) + (d_lambda2 * c2);

        if(strict)
          return strictly_greater(c_prime, c * d);
        return greater_equal(c_prime, c * d);
      }
    }
  }
}

bool inequality::planar_unsatisfiable(std::shared_ptr<inequality> ineq1)
{
  std::vector<std::string> vars_ineq1 = vars(ineq1);
  std::vector<std::string> vars(vars_ineq1.begin(), vars_ineq1.end());

  std::shared_ptr<inequality> i = widen(vars);
  std::shared_ptr<inequality> i1 = ineq1->widen(vars);

  if(
    std::dynamic_pointer_cast<unary_inequality>(i) != nullptr &&
    std::dynamic_pointer_cast<unary_inequality>(i1) != nullptr)
  {
    std::shared_ptr<unary_inequality> u =
      std::dynamic_pointer_cast<unary_inequality>(i);
    std::shared_ptr<unary_inequality> u1 =
      std::dynamic_pointer_cast<unary_inequality>(i1);

    return planar_entailed(true, -u->a, 0, -u->c, u1->a, 0, u1->c);
  }
  else
  {
    std::shared_ptr<dyadic_inequality> d =
      std::dynamic_pointer_cast<dyadic_inequality>(i);
    std::shared_ptr<dyadic_inequality> d1 =
      std::dynamic_pointer_cast<dyadic_inequality>(i1);

    return planar_entailed(true, -d->a, -d->b, -d->c, d1->a, d1->b, d1->c);
  }
}

bool inequality::planar_unsatisfiable(
  std::shared_ptr<inequality> ineq1,
  std::shared_ptr<inequality> ineq2)
{
  std::vector<std::string> vars_ineq1_2 = vars(ineq1, ineq2);
  std::vector<std::string> vars(vars_ineq1_2.begin(), vars_ineq1_2.end());

  std::shared_ptr<inequality> i = widen(vars);
  std::shared_ptr<inequality> i1 = ineq1->widen(vars);
  std::shared_ptr<inequality> i2 = ineq2->widen(vars);

  if(
    std::dynamic_pointer_cast<unary_inequality>(i) != nullptr &&
    std::dynamic_pointer_cast<unary_inequality>(i1) != nullptr &&
    std::dynamic_pointer_cast<unary_inequality>(i2) != nullptr)
  {
    std::shared_ptr<unary_inequality> u =
      std::dynamic_pointer_cast<unary_inequality>(i);
    std::shared_ptr<unary_inequality> u1 =
      std::dynamic_pointer_cast<unary_inequality>(i1);
    std::shared_ptr<unary_inequality> u2 =
      std::dynamic_pointer_cast<unary_inequality>(i2);

    return planar_entailed(
      true, -u->a, 0, -u->c, u1->a, 0, u1->c, u2->a, 0, u2->c);
  }

  else
  {
    std::shared_ptr<dyadic_inequality> d =
      std::dynamic_pointer_cast<dyadic_inequality>(i);
    std::shared_ptr<dyadic_inequality> d1 =
      std::dynamic_pointer_cast<dyadic_inequality>(i1);
    std::shared_ptr<dyadic_inequality> d2 =
      std::dynamic_pointer_cast<dyadic_inequality>(i2);

    return planar_entailed(
      true, -d->a, -d->b, -d->c, d1->a, d1->b, d1->c, d2->a, d2->b, d2->c);
  }
}

bool inequality::less_equal(const mp_integer &a, const mp_integer &b)
{
  return a.compare(b) <= 0;
}

bool inequality::greater_equal(const mp_integer &a, const mp_integer &b)
{
  return less_equal(b, a);
}

bool inequality::strictly_less(const mp_integer &a, const mp_integer &b)
{
  return a.compare(b) == -1;
}

bool inequality::strictly_greater(const mp_integer &a, const mp_integer &b)
{
  return strictly_less(b, a);
}

int inequality::sign(const mp_integer &a)
{
  if(a.is_zero())
    return 0;
  else if(a.is_positive())
    return 1;
  else
    return -1;
}

int compare_to(const mp_integer &a, const mp_integer &b)
{
  if(a == b)
    return 0;
  else if(a > b)
    return 1;
  else
    return -1;
}

bool syntax_compare(
  std::shared_ptr<inequality> a,
  std::shared_ptr<inequality> b)
{
  return a->to_string().compare(b->to_string()) < 0;
}

bool lex_compare(
  std::shared_ptr<inequality> ineq1,
  std::shared_ptr<inequality> ineq2)
{
  if(ineq1->arity() == 2 && ineq2->arity() == 2)
  {
    std::shared_ptr<dyadic_inequality> dyadic1 =
      std::dynamic_pointer_cast<dyadic_inequality>(ineq1);
    std::shared_ptr<dyadic_inequality> dyadic2 =
      std::dynamic_pointer_cast<dyadic_inequality>(ineq2);

    if(dyadic1->x.compare(dyadic2->x) < 0)
      return true;
    else if(dyadic1->x.compare(dyadic2->x) > 0)
      return false;
    else if(dyadic1->y.compare(dyadic2->y) < 0)
      return true;
    else if(dyadic1->y.compare(dyadic2->y) > 0)
      return false;
    else
      return syntax_compare(ineq1, ineq2);
  }
  else if(ineq1->arity() == 1 && ineq2->arity() == 2)
  {
    std::shared_ptr<unary_inequality> unary =
      std::dynamic_pointer_cast<unary_inequality>(ineq1);
    std::shared_ptr<dyadic_inequality> dyadic =
      std::dynamic_pointer_cast<dyadic_inequality>(ineq2);

    if(unary->x.compare(dyadic->x) < 0)
      return true;
    else if(unary->x.compare(dyadic->x) > 0)
      return false;
    else
      return syntax_compare(unary, dyadic);
  }
  else if(ineq1->arity() == 2 && ineq2->arity() == 1)
  {
    return !lex_compare(ineq2, ineq1);
  }
  else if(ineq1->arity() == 1 && ineq2->arity() == 1)
  {
    std::shared_ptr<unary_inequality> unary1 =
      std::dynamic_pointer_cast<unary_inequality>(ineq1);
    std::shared_ptr<unary_inequality> unary2 =
      std::dynamic_pointer_cast<unary_inequality>(ineq2);

    if(unary1->x.compare(unary2->x) < 0)
      return true;
    else if(unary1->x.compare(unary2->x) > 0)
      return false;
    else
      return syntax_compare(unary1, unary2);
  }
  else
  {
    return syntax_compare(ineq1, ineq2);
  }
}

std::string bool_to_string(bool b)
{
  if(b)
    return "true";
  else
    return "false";
}

mp_integer abs(mp_integer n)
{
  if(n.is_negative())
    return -n;
  else
    return n;
}

//haskell extension

/// @brief prints whole tvpi inequality
/// @param tvpi inequality
void show(std::shared_ptr<inequality> i)
{
  if(std::dynamic_pointer_cast<unary_inequality>(i) != nullptr)
  {
    std::shared_ptr<unary_inequality> u =
      std::dynamic_pointer_cast<unary_inequality>(i);
    std::cout << u->a << u->x << "\u2264" << u->c << std::endl;
  }

  else if(std::dynamic_pointer_cast<dyadic_inequality>(i) != nullptr)
  {
    std::string sign = "";
    std::shared_ptr<dyadic_inequality> d =
      std::dynamic_pointer_cast<dyadic_inequality>(i);
    if(d->b >= 0)
      sign = "+";
    std::cout << d->a << d->x << sign << d->b << d->y << "\u2264" << d->c
              << std::endl;
  }

  else if(std::dynamic_pointer_cast<constant_inequality>(i) != nullptr)
  {
    std::shared_ptr<constant_inequality> c =
      std::dynamic_pointer_cast<constant_inequality>(i);
    std::cout << c->sat << std::endl;
  }
}

/// @brief fully prints vector of tvpi inequalities
/// @param xs vector of tvpi inequalities
void print_cons(std::vector<std::shared_ptr<inequality>> &xs)
{
  for(size_t i = 0; i < xs.size(); i++)
  {
    show(xs[i]);
  }
  if(xs.empty())
  {
    std::cout << "[]" << std::endl;
  }
}

void normalize_coeff(std::vector<std::shared_ptr<inequality>> &xs)
{
  for(size_t i = 0; i < xs.size(); i++)
  {
    if(std::dynamic_pointer_cast<unary_inequality>(xs[i]) != nullptr)
    {
      std::shared_ptr<unary_inequality> u =
        std::dynamic_pointer_cast<unary_inequality>(xs[i]);
      std::shared_ptr<dyadic_inequality> d;

      //check what label is contained already
      //then create a dyadic inequality

      if(u->x == "x")
      {
        d = std::make_shared<dyadic_inequality>(u->x, "y", u->a, 0, u->c);
      }
      if(u->x == "y")
      {
        d = std::make_shared<dyadic_inequality>("x", u->x, 0, u->a, u->c);
      }

      xs[i] = d;
    }
  }
}

inequality::direction inequality::calc_direction()
{
  mp_integer a, b, c;
  std::string x, y;

  std::shared_ptr<inequality> i = widen(vars());

  if(std::dynamic_pointer_cast<dyadic_inequality>(i) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> d =
      std::dynamic_pointer_cast<dyadic_inequality>(i);
    a = d->a;
    b = d->b;
    c = d->c;
    x = d->x;
    y = d->y;
  }

  //std::cout<<a<<b<<c<<x<<y<<std::endl;

  return (a > 0)
           ? ((b >= 0) ? East : South)
           : ((a < 0) ? ((b > 0) ? North : West) : ((b > 0) ? North : South));
}

void print_direction(inequality::direction d)
{
  std::string label;
  if(d == inequality::East)
    label = "East";
  else if(d == inequality::North)
    label = "North";
  else if(d == inequality::West)
    label = "West";
  else if(d == inequality::South)
    label = "South";

  std::cout << label << std::endl;
}

void print_ordering(inequality::ordering o)
{
  std::string label;
  if(o == inequality::LT)
    label = "LESS";
  else if(o == inequality::EQ)
    label = "EQUAL";
  else if(o == inequality::GT)
    label = "GREATER";
  std::cout << label << std::endl;
}

inequality::ordering
cmp_angle(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b)
{
  inequality::ordering res = inequality::EQ;

  if(
    std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(b) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);
    std::shared_ptr<dyadic_inequality> i2 =
      std::dynamic_pointer_cast<dyadic_inequality>(b);

    inequality::direction dir_a = a->calc_direction();
    inequality::direction dir_b = b->calc_direction();

    if(dir_a == dir_b)
    {
      if((i2->a * i1->b) == (i1->a * i2->b))
      {
        return res;
      }
      else if((i2->a * i1->b) > (i1->a * i2->b))
      {
        res = inequality::GT;
      }
      else
      {
        res = inequality::LT;
      }
    }
    else
    {
      if(dir_a > dir_b)
        res = inequality::GT;
      else
        res = inequality::LT;
    }
  }

  return res;
}

mp_integer gcd_vector(std::vector<mp_integer> v)
{
  mp_integer res = 1;
  if(v.size() == 1)
    res = v[0];

  else
  {
    for(size_t i = 2; i < v.size(); i++)
    {
      res = gcd(v[0], v[1]);
      res = gcd(res, v[i]);
    }
  }

  return res;
}

mp_integer
determinant(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b)
{
  mp_integer det;

  if(
    std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(b) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);
    std::shared_ptr<dyadic_inequality> i2 =
      std::dynamic_pointer_cast<dyadic_inequality>(b);

    det = (i1->a * i2->b) - (i2->a * i1->b);
  }

  return det;
}

bool equal_to_pi(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b)
{
  mp_integer det = determinant(a, b);

  int diff = abs(b->calc_direction() - a->calc_direction() + 4) % 4;

  //std::cout<<"diff: "<<diff<<std::endl;

  return diff == 2 && det == 0;
}

bool less_than_pi(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b)
{
  mp_integer det = determinant(a, b);

  int diff = abs(b->calc_direction() - a->calc_direction() + 4) % 4;

  //std::cout<<b->calc_direction()<<std::endl;
  //std::cout<<a->calc_direction()<<std::endl;

  //std::cout<<"diff: "<<diff<<std::endl;
  //std::cout<<"det: "<<det<<std::endl;

  if(diff == 0 && det >= 0)
    return true;
  else if(diff == 2 && det > 0)
    return true;
  else if(diff == 1)
    return true;
  else
    return false;
}

rationalt dist_to_ineq(vertex ver, std::shared_ptr<inequality> a)
{
  rationalt dis;

  if(std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);

    dis =
      ver.x * rationalt(i1->a) + ver.y * rationalt(i1->b) - rationalt(i1->c);
  }

  return dis;
}

vertex
calc_intersection(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b)
{
  rationalt x;
  rationalt y;

  if(
    std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(b) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);
    std::shared_ptr<dyadic_inequality> i2 =
      std::dynamic_pointer_cast<dyadic_inequality>(b);

    rationalt det_ab = rationalt(i1->a * i2->b - i1->b * i2->a);

    //std::cout<<det_ab<<std::endl;

    x = rationalt((i1->c * i2->b - i1->b * i2->c)) / det_ab;

    y = rationalt((i1->a * i2->c - i1->c * i2->a)) / det_ab;
  }

  return vertex(x, y);
}

rationalt
calc_shift(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b)
{
  mp_integer f1 = 0;
  mp_integer f2 = 0;
  rationalt shift;

  if(
    std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(b) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);
    std::shared_ptr<dyadic_inequality> i2 =
      std::dynamic_pointer_cast<dyadic_inequality>(b);

      std::cout<<"inside shift"<<std::endl;
      std::cout<<i1->to_string()<<std::endl;
      std::cout<<i2->to_string()<<std::endl;

    if(i1->a.is_zero() || i2->a.is_zero())
    {
      f1 = i1->b;
      f2 = i2->b;
    }
    else
    {
      f1 = i1->a;
      f2 = i2->a;
    }

    std::cout<<f1<<" "<<f2<<std::endl;

    assert(!f1.is_zero());
    assert(!f2.is_zero());

    shift =rationalt(i2->c * abs(f1) - i1->c * abs(f2)) / rationalt(abs(f1 * f2));
    
    std::cout<<shift<<std::endl;
  }
  return shift;
  //return shift * rationalt(f1);

}

std::shared_ptr<inequality>
add_shift(rationalt r, std::shared_ptr<inequality> a)
{
  std::string x, y;
  mp_integer _a, _b, _c;
  mp_integer new_a, new_b, new_c, d;

  if(std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);

    x = i1->x;
    y = i1->y;

    mp_integer nf = r.get_numerator();
    mp_integer df = r.get_denominator();

    new_a = df * i1->a;
    new_b = df * i1->b;
    new_c = df * i1->c + nf;

    std::vector<mp_integer> new_coefs;

    new_coefs.push_back(abs(new_a));
    new_coefs.push_back(abs(new_b));
    new_coefs.push_back(abs(new_c));

    new_coefs.erase(
      std::remove_if(
        new_coefs.begin(),
        new_coefs.end(),
        [](mp_integer i) { return i.is_zero(); }),
      new_coefs.end());

    d = gcd_vector(new_coefs);
  }

  return std::make_shared<dyadic_inequality>(
    x, y, new_a / d, new_b / d, new_c / d);
}

std::optional<rationalt> calc_dist(
  std::shared_ptr<inequality> a,
  std::shared_ptr<inequality> e,
  std::shared_ptr<inequality> b)
{
  if(
    std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(e) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(b) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);
    std::shared_ptr<dyadic_inequality> i2 =
      std::dynamic_pointer_cast<dyadic_inequality>(b);
    std::shared_ptr<dyadic_inequality> it =
      std::dynamic_pointer_cast<dyadic_inequality>(e);

    
    std::cout<<"calc_dist: i1: "<<i1->to_string()<<" i2: "<<i2->to_string()<<" e: "<<it->to_string()<<std::endl;

    if(less_than_pi(i1, i2) && cmp_angle(i1, i2) != inequality::EQ)
    {
      return dist_to_ineq(calc_intersection(i1, i2), it);
    }
    if(cmp_angle(i1, it) == inequality::EQ)
    {
      return calc_shift(it, i1);
    }
    if(cmp_angle(i2, it) == inequality::EQ)
    {
      return calc_shift(it, i2);
    }
  }
  return {};
}

bool inbetween(
  std::shared_ptr<inequality> a,
  std::shared_ptr<inequality> e,
  std::shared_ptr<inequality> b)
{
  bool result = false;

  if(
    std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(e) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(b) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);
    std::shared_ptr<dyadic_inequality> i2 =
      std::dynamic_pointer_cast<dyadic_inequality>(b);
    std::shared_ptr<dyadic_inequality> it =
      std::dynamic_pointer_cast<dyadic_inequality>(e);

    if(cmp_angle(i1, i2) == inequality::EQ)
      result = true;
    else
    {
      bool r1 = less_than_pi(i1, it);
      bool r2 = less_than_pi(it, i2);

      if(r1 && r2)
        result = determinant(i1, it) >= 0 && determinant(it, i2) >= 0;
      else if(r1 && !r2)
        result = determinant(i2, i1) >= 0;
      else if(!r1 && r2)
        result = determinant(i2, i1) >= 0;
      else if(!r1 && !r2)
        result = equal_to_pi(i1, i2) && determinant(i1, it) >= 0;
    }
  }

  return result;
}

bool less_equal_zero(std::optional<rationalt> r)
{
  if(r.has_value())
  {
    auto num = r.value().get_numerator();
    auto den = r.value().get_denominator();

    return (num <= 0) ^ (den <= 0);
  }
  else
    return false;
}

bool greater_equal_zero(std::optional<rationalt> r)
{
  if(r.has_value())
  {
    auto num = r.value().get_numerator();
    auto den = r.value().get_denominator();

    std::cout<<"numerator here: "<<num<<std::endl;
    std::cout<<"denominator here: "<<den<<std::endl;

    auto product = num * den;
    std::cout<<"product :"<<product<<std::endl;
    
     return product >= 0;
  }
  else if(!r.has_value())
  {
    return true;
  }
  else
    return false;
}

void rm_taut(std::vector<std::shared_ptr<inequality>> &cs)
{
  cs.erase(
    std::remove_if(
      cs.begin(),
      cs.end(),
      [](std::shared_ptr<inequality> a)
      {
        if(std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr)
        {
          std::shared_ptr<dyadic_inequality> i =
            std::dynamic_pointer_cast<dyadic_inequality>(a);

          return i->a.is_zero() && i->b.is_zero();
        }
        else
        {
          return false;
        }
      }),
    cs.end());
}

std::vector<std::shared_ptr<inequality>> rm_syn_red(std::vector<std::shared_ptr<inequality>> cs)
{ 
  //constraints without consecutive duplicates
  std::vector<std::shared_ptr<inequality>> clean_cs;

  if(cs.size()<=1){
    return cs;
  }

  std::shared_ptr<inequality> ref_ineq = cs[0];
  std::shared_ptr<inequality> cur_ineq;

  clean_cs.push_back(ref_ineq);

  for(auto it = cs.begin()+1; it< cs.end();it++){

   cur_ineq = *it;
   
   if(cur_ineq->to_string()!=ref_ineq->to_string()){
    clean_cs.push_back(cur_ineq);
    ref_ineq=cur_ineq;
   }

  }

  return clean_cs;
}

std::vector<std::shared_ptr<inequality>>
rm_local_red(std::vector<std::shared_ptr<inequality>> &cs)
{
  std::vector<std::shared_ptr<inequality>> res;

  //original end of the input
  //always the same
  size_t end = cs.size() - 1;
  size_t cur = 0;
  size_t prev = end;
  size_t x = cur + 1;
  size_t last_deleted = 0;

  if(cs.empty())
  {
    return res;
  }

  else if(cs.size() == 1)
  {
    return cs;
  }

  while(cur < end + 1)
  {
    std::cout << "prev: " << prev << " cur: " << cur << " x: " << x
              << std::endl;
    if(is_redundant(cs[prev], cs[cur], cs[x]))
    {
      std::cout << "The redundant inequality is: ";
      show(cs[cur]);
      last_deleted = cur + 1;

      x++;
      cur++;
    }

    else
    {
      res.push_back(cs[cur]);

      if(last_deleted != 0)
      {
        prev = last_deleted;
        last_deleted = 0;
      }
      else
        prev++;

      cur++;
      x++;
    }

    //guards
    if(prev > end)
      prev = 0;
    if(x > end)
      x = 0;
  }

  //std::cout<<"local res"<<std::endl;
  //print_cons(res);

  return res;
}

std::optional<std::shared_ptr<inequality>> delta_combination(
  rationalt fr1,
  std::shared_ptr<inequality> a,
  rationalt fr2,
  std::shared_ptr<inequality> b)
{
  mp_integer check = (fr1.get_numerator() * fr2.get_numerator()) /
                     (fr1.get_denominator() * fr2.get_denominator());

  assert(check >= 0);

  std::cout<<"inside delta combination"<<std::endl;
  mp_integer n1, n2, d1, d2;
  mp_integer new_a, new_b, new_c, d;
  std::string x, y;

  //haskell fix sign from denominator jumps to numerator
  n1 = fr1.get_numerator();
  d1 = fr1.get_denominator();
  if(d1.is_negative()){
    n1 = n1.negate();
    d1 = d1.negate();
  }
  n2 = fr2.get_numerator();
  d2 = fr2.get_denominator();
   if(d2.is_negative()){
    n2 = n2.negate();
    d2 = d2.negate();
  }

  //std::cout<<n1<<" "<<d1<<" "<<n2<<" "<<d2<<" "<<std::endl;

  if(
    std::dynamic_pointer_cast<dyadic_inequality>(a) != nullptr &&
    std::dynamic_pointer_cast<dyadic_inequality>(b) != nullptr)
  {
    std::shared_ptr<dyadic_inequality> i1 =
      std::dynamic_pointer_cast<dyadic_inequality>(a);
    std::shared_ptr<dyadic_inequality> i2 =
      std::dynamic_pointer_cast<dyadic_inequality>(b);

    x = i1->x;
    y = i1->y;


    new_a = abs(n2) * d1 * i1->a + abs(n1) * d2 * i2->a;
    new_b = abs(n2) * d1 * i1->b + abs(n1) * d2 * i2->b;
    new_c = abs(n2) * d1 * i1->c + abs(n1) * d2 * i2->c + abs(n1) * n2;

    std::vector<mp_integer> new_coefs;

    new_coefs.push_back(abs(new_a));
    new_coefs.push_back(abs(new_b));
    new_coefs.push_back(abs(new_c));

    new_coefs.erase(
      std::remove_if(
        new_coefs.begin(),
        new_coefs.end(),
        [](mp_integer i) { return i.is_zero(); }),
      new_coefs.end());

    d = gcd_vector(new_coefs);
  }
  std::cout<<"d is:"<<d<<std::endl;


  if(new_a.is_zero() && new_b.is_zero())
  {
    return {};
  }
  else
  {
    return std::make_shared<dyadic_inequality>(
      x, y, new_a / d, new_b / d, new_c / d);
  }
}

std::vector<std::shared_ptr<inequality>> gen_delta_comb(
  std::optional<rationalt> mDInner,
  std::shared_ptr<inequality> oC,
  std::optional<rationalt> mDOuter,
  std::shared_ptr<inequality> iP,
  std::vector<std::shared_ptr<inequality>> &other)
{
  std::cout << "inside gen delta: " << " oC: " << oC->to_string() << " iP "
            << iP->to_string() << std::endl;
  if(mDInner.has_value() && mDOuter.has_value())
  {
    std::cout << " mDInner: " << mDInner.value()
              << " mDOuter: " << mDOuter.value() << std::endl;
  }

  if(cmp_angle(oC, iP) == inequality::EQ)
  {
    if(cmp_angle(oC, iP) == inequality::GT)
    {
      other.push_back(oC);
    }
    else
    {
      other.push_back(iP);
    }
  }
  else
  {
    if(mDInner.has_value() && mDOuter.has_value())
    {
      auto dc = delta_combination(mDInner.value(), oC, mDOuter.value(), iP);

      if(dc.has_value())
      {
        other.push_back(dc.value());
      }
      else
      {
        return other;
        // ??? Is this unreachable?
        UNREACHABLE;
      }
    }
    else if(mDInner.has_value())
    {
      auto shifted = add_shift(mDInner.value(), oC);
      other.push_back(shifted);
    }
    else if(mDOuter.has_value())
    {
      auto shifted = add_shift(mDOuter.value(), iP);
      other.push_back(shifted);
    }
    else
    {
      return other;
      // ??? Is this unreachable?
      UNREACHABLE;
    }
  }

  return other;
}

void show_details(
  std::shared_ptr<inequality> iP,
  std::shared_ptr<inequality> iC,
  std::vector<std::shared_ptr<inequality>> is,
  std::shared_ptr<inequality> oP,
  std::shared_ptr<inequality> oC,
  std::vector<std::shared_ptr<inequality>> os,
  std::string label,
  std::optional<rationalt> d)
{
  std::cout << "iP: ";
  show(iP);
  std::cout << "iC: ";
  show(iC);

  /*
		std::cout<<std::endl;
		std::cout<<"is: "<<std::endl;
		std::vector<std::shared_ptr<inequality>> is_tail(is.begin()+1,is.end());
		print_cons(is_tail);
		std::cout<<std::endl;
		*/

  std::cout << "oP: ";
  show(oP);
  std::cout << "oC: ";
  show(oC);

  /*
		std::cout<<"os: "<<std::endl;
		std::vector<std::shared_ptr<inequality>> os_tail(os.begin()+1,os.end());
		print_cons(os_tail);
		std::cout<<std::endl;

		if(!label.empty()) std::cout<<label;
		else std::cout<<"N/A: ";
		if(d.has_value()){
			std::cout<<d.value()<<std::endl;
		}
		else{
			std::cout<<"Nothing"<<std::endl;
		}
		std::cout<<std::endl;

		*/
}

void advance_outer(
  std::vector<std::shared_ptr<inequality>> &res,
  std::shared_ptr<inequality> iP,
  std::vector<std::shared_ptr<inequality>> &is,
  std::shared_ptr<inequality> oP,
  std::vector<std::shared_ptr<inequality>> &os)
{
  //std::cout<<"res in outer"<<std::endl;
  //print_cons(res);

  //hull::full_hull_trace<<"res in outer"<<std::endl;

  if(!is.empty() && !os.empty())
  {
    join::hull_ID += 1;
    auto iC = is[0];
    auto oC = os[0];
    join::full_hull_trace << "outer: " << "iP: " << iP->to_string()
                          << " oC: " << oC->to_string()
                          << " iC: " << iC->to_string() << std::endl;

    /* TODO: improve
    tikz_printer::print_tikz_hull(
      {iP},
      {oC},
      {iC},
      "../../trace/" + std::to_string(hull::hull_ID) + ".tex",
      "red",
      "blue",
      "red");
    */

    std::cout << "outer: " << "iP: " << iP->to_string()
                          << " oC: " << oC->to_string()
                          << " iC: " << iC->to_string() << std::endl;

    if(inbetween(iP, oC, iC))
    {

      auto mDInner = calc_dist(iP, oC, iC);

      if(less_equal_zero(mDInner))
      {
        std::cout << std::endl;
        std::cout << "OUT2 " << std::endl;

        show_details(iP, iC, is, oP, oC, os, "mDInner: ", mDInner);

        //retained edge
        std::cout << "retained edge is:" << std::endl;
        show(oC);

        res.push_back(oC);
        os.erase(os.begin());

        advance_inner(res, iP, is, oC, os);
      }
      else
      {  
        auto mDOuter = calc_dist(oP, iP, oC);
        std::cout << std::endl;
        std::cout << "OUT3 " << std::endl;
        show_details(iP, iC, is, oP, oC, os, "mDOuter: ", mDOuter);

        if(mDOuter.has_value()){
            std::cout<<mDOuter.value()<<std::endl;
        }
        else{
          std::cout<<"mDOuter does not exist"<<std::endl;
        }
         if(mDInner.has_value()){
            std::cout<<mDInner.value()<<std::endl;
        }
        else{
          std::cout<<"mDInner does not exist"<<std::endl;
        }

        //gen fix
        auto new_cons = gen_delta_comb(mDInner,oC,mDOuter, iP,res);
        //fix?
        //assert(new_cons.size() > 0);

        //new edge
        if(!new_cons.empty()){
          std::cout << "new edge is:" << std::endl;
          show(new_cons.back());
        }
        
        advance_inner(res, oP, os, iP, is);
      }
    }
    else
    {
      std::cout << std::endl;
      std::cout << "OUT4 " << std::endl;
      show_details(iP, iC, is, oP, oC, os, "", rationalt(0));
      advance_inner(res, iP, is, oP, os);
    }
  }
}

void advance_inner(
  std::vector<std::shared_ptr<inequality>> &res,
  std::shared_ptr<inequality> iP,
  std::vector<std::shared_ptr<inequality>> &is,
  std::shared_ptr<inequality> oP,
  std::vector<std::shared_ptr<inequality>> &os)
{
  //std::cout<<"res in inner"<<std::endl;
  //print_cons(res);
  //hull::full_hull_trace<<"res in inner"<<std::endl;
  if(!os.empty() && !is.empty())
  {
    auto oC = os[0];
    auto iC = is[0];

    join::hull_ID += 1;
    /* TODO: improve
    tikz_printer::print_tikz_hull(
      {oP},
      {iC},
      {oC},
      "../../trace/" + std::to_string(hull::hull_ID) + ".tex",
      "red",
      "blue",
      "red");
    */
    join::full_hull_trace << "inner: " << "oP: " << oP->to_string()
                          << " iC: " << iC->to_string()
                          << " oC: " << oC->to_string() << std::endl;

    if(inbetween(oP, iC, oC))
    {
      auto mDOuter = calc_dist(oP, iC, oC);
      if(mDOuter.has_value()){
        std::cout<<"mdOuter here:"<< mDOuter.value() <<std::endl;
        std::cout<<"greater or equal"<< greater_equal_zero(mDOuter.value())<<std::endl;
      }

      //if(greater_equal_zero(mDOuter))
      rationalt num, den;
      mp_integer result;
      if(mDOuter.has_value()){
      result = mDOuter.value().get_denominator() *mDOuter.value().get_numerator();
        //den = mDOuter.value().get_denominator();
        //num = mDOuter.value().get_numerator();

      }
     if(result>0||!mDOuter.has_value())
     //if(greater_equal_zero(mDOuter))
      {
        std::cout << std::endl;
        std::cout << "IN2 " << std::endl;

        show_details(iP, iC, is, oP, oC, os, "mDOuter: ", mDOuter);

        is.erase(is.begin());

        advance_outer(res, iC, is, oP, os);
      }

      else
      {
        auto mDInner = calc_dist(iP, oP, iC);
        std::cout << std::endl;
        std::cout << "IN3 " << std::endl;
        show_details(iP, iC, is, oP, oC, os, "mDInner: ", mDInner);

        auto new_cons = gen_delta_comb(mDOuter, iC, mDInner, oP, res);

        //new edge
        if(!new_cons.empty()){
         std::cout << "new edge is:" << std::endl;
         show(new_cons.back());
        }

        advance_outer(res, oP, os, iP, is);
      }
    }
    else
    {
      std::cout << std::endl;
      std::cout << "IN4 " << std::endl;
      show_details(iP, iC, is, oP, oC, os, "", rationalt(0));

      advance_outer(res, iP, is, oP, os);
    }
  }
}

extern bool is_redundant(
  std::shared_ptr<inequality> a,
  std::shared_ptr<inequality> b,
  std::shared_ptr<inequality> c)
{
  
if(calc_dist(a,b,c).has_value()){
	return less_equal_zero(calc_dist(a,b,c).value());
}
else{
	return false;
}


  //return b->planar_entailed(a, c);
}

extern bool is_negative(
  rationalt rn)
{
    mp_integer product = rn.get_numerator() * rn.get_denominator();
    return !rn.is_zero() && product<0;
}
//haskell extension
