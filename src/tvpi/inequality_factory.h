#ifndef INEQUALITY_FACTORY_H
#define INEQUALITY_FACTORY_H

#include "inequality.h"

class inequality_factory{

    public:

    static std::shared_ptr<inequality> makeInequality(std::string x, std::string y, int a, int b, int c);
    static std::shared_ptr<inequality> makeInequality(std::string x, int a, int c);
    static std::shared_ptr<inequality> make_inequality(std::string x,std::string y, mp_integer a, mp_integer b, mp_integer c);
    static std::shared_ptr<inequality> make_inequality(std::string x, mp_integer a, mp_integer c);
    static std::shared_ptr<inequality> make_inequality(mp_integer c);

};

#endif