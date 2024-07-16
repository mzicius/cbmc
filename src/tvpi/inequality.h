#ifndef INEQUALITY_H
#define INEQUALITY_H

#include <util/mp_arith.h>

#include "coordinate.h"
#include "vertex.h"

#include <assert.h>
#include <memory>
#include <set>
#include <vector>

class inequality
{
public:
  //haskell extension
  enum direction
  {
    East,
    North,
    West,
    South
  };

  enum ordering
  {
    LT,
    EQ,
    GT
  };

  direction calc_direction();

  void print_direction(direction d);

  //haskell extension

  virtual ~inequality() = default;

  virtual std::vector<std::string> vars() = 0;

  std::vector<std::string> vars(std::shared_ptr<inequality> i1);

  std::vector<std::string>
  vars(std::shared_ptr<inequality> i1, std::shared_ptr<inequality> i2);

  virtual std::vector<coordinate> intercepts(
    std::string x,
    std::string y,
    int x_min,
    int y_min,
    int x_max,
    int y_max) = 0;

  virtual std::vector<coordinate> halfspace(
    std::string x,
    std::string y,
    int x_min,
    int y_min,
    int x_max,
    int y_max) = 0;

  int arity();

  virtual std::shared_ptr<inequality> widen(std::vector<std::string> vars) = 0;

  virtual std::string to_string() = 0;

  virtual std::string to_ltx_label() = 0;

  bool planar_entailed(std::shared_ptr<inequality> ineq1);

  bool planar_entailed(
    std::shared_ptr<inequality> ineq1,
    std::shared_ptr<inequality> ineq2);

  bool planar_unsatisfiable(std::shared_ptr<inequality> ineq1);

  bool planar_unsatisfiable(
    std::shared_ptr<inequality> ineq1,
    std::shared_ptr<inequality> ineq2);

private:
  bool planar_entailed(
    bool strict,
    mp_integer a,
    mp_integer b,
    mp_integer c,
    mp_integer a1,
    mp_integer b1,
    mp_integer c1);

  bool planar_entailed(
    bool strict,
    mp_integer a,
    mp_integer b,
    mp_integer c,
    mp_integer a1,
    mp_integer b1,
    mp_integer c1,
    mp_integer a2,
    mp_integer b2,
    mp_integer c2);

  bool const debugging = false;

protected:
  int sign(const mp_integer &a);

  bool less_equal(const mp_integer &a, const mp_integer &b);

  bool greater_equal(const mp_integer &a, const mp_integer &b);

  bool strictly_less(const mp_integer &a, const mp_integer &b);

  bool strictly_greater(const mp_integer &a, const mp_integer &b);
};

extern bool
syntax_compare(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b);

extern bool lex_compare(
  std::shared_ptr<inequality> ineq1,
  std::shared_ptr<inequality> ineq2);

extern int compare_to(const mp_integer &a, const mp_integer &b);

extern std::string bool_to_string(const bool b);

extern mp_integer abs(mp_integer n);

extern void print_vars(const std::vector<std::string> &sts);

extern void print_ins(const std::vector<std::shared_ptr<inequality>> ins);

//haskell extension

extern void show(std::shared_ptr<inequality> i);

extern void print_cons(std::vector<std::shared_ptr<inequality>> &xs);

extern void normalize_coeff(std::vector<std::shared_ptr<inequality>> &xs);

extern void print_direction(inequality::direction d);

extern void print_ordering(inequality::ordering o);

extern inequality::ordering
cmp_angle(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b);

extern mp_integer gcd_vector(std::vector<mp_integer>);

extern mp_integer
determinant(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b);

extern bool
equal_to_pi(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b);

extern bool
less_than_pi(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b);

extern rationalt dist_to_ineq(vertex ver, std::shared_ptr<inequality> a);

extern vertex
calc_intersection(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b);

extern rationalt
calc_shift(std::shared_ptr<inequality> a, std::shared_ptr<inequality> b);

extern std::shared_ptr<inequality>
add_shift(rationalt r, std::shared_ptr<inequality> a);

extern std::optional<rationalt> calc_dist(
  std::shared_ptr<inequality> a,
  std::shared_ptr<inequality> e,
  std::shared_ptr<inequality> b);

extern bool inbetween(
  std::shared_ptr<inequality> a,
  std::shared_ptr<inequality> e,
  std::shared_ptr<inequality> b);

extern bool less_equal_zero(std::optional<rationalt> r);

extern bool greater_equal_zero(std::optional<rationalt> r);

extern void rm_taut(std::vector<std::shared_ptr<inequality>> &cs);

extern std::vector<std::shared_ptr<inequality>> rm_syn_red(std::vector<std::shared_ptr<inequality>> cs);

extern std::vector<std::shared_ptr<inequality>>
rm_local_red(std::vector<std::shared_ptr<inequality>> &cs);

extern std::optional<std::shared_ptr<inequality>> delta_combination(
  rationalt fr1,
  std::shared_ptr<inequality> a,
  rationalt fr2,
  std::shared_ptr<inequality> b);

extern std::vector<std::shared_ptr<inequality>> gen_delta_comb(
  std::optional<rationalt> mDInner,
  std::shared_ptr<inequality> oC,
  std::optional<rationalt> mDOuter,
  std::shared_ptr<inequality> iP,
  std::vector<std::shared_ptr<inequality>> &other);

void advance_outer(
  std::vector<std::shared_ptr<inequality>> &res,
  std::shared_ptr<inequality> iP,
  std::vector<std::shared_ptr<inequality>> &is,
  std::shared_ptr<inequality> oP,
  std::vector<std::shared_ptr<inequality>> &os);

void advance_inner(
  std::vector<std::shared_ptr<inequality>> &res,
  std::shared_ptr<inequality> iP,
  std::vector<std::shared_ptr<inequality>> &is,
  std::shared_ptr<inequality> oP,
  std::vector<std::shared_ptr<inequality>> &os);

extern bool is_redundant(
  std::shared_ptr<inequality> a,
  std::shared_ptr<inequality> b,
  std::shared_ptr<inequality> c);

extern bool is_negative(
  rationalt rn);

//end of haskell extension

#endif
