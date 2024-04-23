#ifndef PLANAR_COMPARATOR_H
#define PLANAR_COMPARATOR_H

#include "inequality.h"


class planar_comparator {
    
    public:

    planar_comparator(std::vector<std::string> _vars);
    static int oct_class(mp_integer a, mp_integer b);
    static bool compare_angle(mp_integer a, mp_integer b, mp_integer a_prime, mp_integer b_prime);
    bool operator()(std::shared_ptr<inequality> ineq1, std::shared_ptr<inequality> ineq2);

    private:

    std::vector<std::string> vars;
    static bool const debugging = false;

};
    

#endif
