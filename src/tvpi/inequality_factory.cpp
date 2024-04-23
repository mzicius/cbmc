#include "inequality_factory.h"
#include "constant_inequality.h"
#include "unary_inequality.h"



/*std::shared_ptr<inequality> inequality_factory::makeInequality(std::string x, std::string y, int a, int b, int c) {
        return makeInequality(x, y, a, b, c);
}

std::shared_ptr<inequality> inequality_factory::makeInequality(std::string x, int a, int c){
        return makeInequality(x, a, c);
}
*/

std::shared_ptr<inequality> inequality_factory::make_inequality(std::string x,std::string y, mp_integer a, mp_integer b, mp_integer c){
	if (compare_to(a,0) == 0) return make_inequality(y, b, c);
	else if (compare_to(b, 0) == 0) return make_inequality(x, a, c);
	else if (x.compare(y) == 0) return make_inequality(x, (a+b), c);
	else if (x.compare(y) > 0) return make_inequality(y, x, b, a, c);
	else return std::make_shared<dyadic_inequality>(x, y, a, b, c);
}

std::shared_ptr<inequality> inequality_factory::make_inequality(std::string x, mp_integer a, mp_integer c){
	if (a.is_zero()) return make_inequality(c);
	else return std::make_shared<unary_inequality>(x, a, c);
}

std::shared_ptr<inequality> inequality_factory::make_inequality(mp_integer c){
    return std::make_shared<constant_inequality>(0 <= compare_to(c,0));
}


