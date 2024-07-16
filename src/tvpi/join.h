#ifndef JOIN_H
#define JOIN_H

#include "inequality.h"
#include "parser.h"

class join
{
public:
  static void rm_excess(
    std::shared_ptr<inequality> ref,
    std::vector<std::shared_ptr<inequality>> &ls);

  static std::vector<std::shared_ptr<inequality>> calc_hull(
    std::vector<std::shared_ptr<inequality>> xs,
    std::vector<std::shared_ptr<inequality>> ys);

  static void
  merge(std::string in_dir_xs, std::string in_dir_ys, std::string out_dir);

  static std::ofstream full_hull_trace;
  static int hull_ID;
};

#endif