#include "inequality_factory.h"

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

std::shared_ptr<inequality> inequality_factory::make_inequality(std::string x,std::string y, rationalt a, rationalt b, rationalt c){

	mp_integer a_num, a_den, b_num, b_den, c_num, c_den;

	a_num = a.get_numerator();
	a_den = a.get_denominator();
	b_num = b.get_numerator();
	b_den = b.get_denominator();
	c_num = c.get_numerator();
	c_den = c.get_denominator();

	rationalt sum_ab =  a+b;
	mp_integer ab_num, ab_den;

	ab_num = sum_ab.get_numerator();
	ab_den = sum_ab.get_denominator();

	if (a==rationalt(0)) return make_inequality(y, (b_num*(b_den*c_den))/b_den, (c_num*(b_den*c_den))/c_den);
	else if (b==rationalt(0)) return make_inequality(x, (a_num*(a_den*c_den))/a_den, (c_num*(a_den*c_den))/c_den);
	else if (x.compare(y) == 0) return make_inequality(x,(ab_num*(ab_den*c_den))/ab_den, (c_num*(ab_den*c_den))/c_den);
	else if (x.compare(y) > 0) return make_inequality(y, x, (b_num * (a_den*b_den*c_den))/b_den, (a_num * (a_den*b_den*c_den))/a_den, (c_num * (a_den*b_den*c_den))/c_den);
	else return std::make_shared<dyadic_inequality>(x, y, (a_num * (a_den*b_den*c_den))/a_den, (b_num * (a_den*b_den*c_den))/b_den, (c_num * (a_den*b_den*c_den))/c_den);
}

std::shared_ptr<inequality> inequality_factory::make_inequality(std::string x, rationalt a, rationalt c){
	
	mp_integer a_num  = a.get_numerator();
	mp_integer a_den  = a.get_denominator();
	
	mp_integer c_num  = c.get_numerator();
	mp_integer c_den  = c.get_denominator();

	if (a.is_zero()) return make_inequality(c_num/c_den);
	else return std::make_shared<unary_inequality>(x, (a_num*(a_den*c_den))/a_den, (c_num*(a_den*c_den))/c_den);
}
