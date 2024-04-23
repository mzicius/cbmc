#ifndef HULL_H
#define HULL_H

#include "inequality.h"
#include "parser.h"

class hull{

public:

static void rm_excess(std::shared_ptr<inequality> ref, std::vector<std::shared_ptr<inequality>> &ls);

static std::vector<std::shared_ptr<inequality>> calc_hull(std::vector<std::shared_ptr<inequality>> xs, std::vector<std::shared_ptr<inequality>> ys);

static void join(std::string in_dir_xs, std::string in_dir_ys, std::string out_dir);

};

#endif