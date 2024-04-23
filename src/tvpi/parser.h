#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <random>
#include "inequality_factory.h"
#include "constant_inequality.h"
#include "unary_inequality.h"
#include "complete.h"

namespace fs = std::filesystem;

class parser{

public:

//static std::vector<std::string> get_locations(std::string dir);

static std::vector<std::shared_ptr<inequality>> parse_from_csv(std::string read_dir);

static std::shared_ptr<inequality> parse_inequality(std::string line);

static void complete_system(std::string read_dir,std::string file,std::string dir);

};

#endif
