#include "planar_comparator.h"
#include "unary_inequality.h"
#include <vector>
#include <iostream>

planar_comparator::planar_comparator(std::vector<std::string> _vars){
	
	if(debugging){
	std::cout<<"vars in planar_comparator:";
	for (std::string s : _vars)
        std::cout << s + " ";
    std::cout<<std::endl;
	}

    if (_vars.size() > 2) vars = {};
	else vars = _vars;
 }

/*  angle normal to half-space in the direction of infeasibility
    0 = [0, pi/2), 1 = [pi/2, pi), 2 = [pi, 3pi/2), 3 = [3pi/2, 2pi)  */

int planar_comparator::oct_class(mp_integer a, mp_integer b){


    int a_sign = compare_to(a,0);
	int b_sign = compare_to(b,0);
	int oct_class = 1;

	/*
    Ordering in book does not square with definition of angle
	if (a_sign < 0) oct_class = 7 - b_sign;
	else if (a_sign == 0)
	{  
           if (b_sign <= 0) oct_class = 1;
	   else oct_class = 5;
        }
	else oct_class = 3 + b_sign; 
    */

	if      (a_sign  > 0 && b_sign == 0) oct_class = 0;
	else if (a_sign  > 0 && b_sign  > 0) oct_class = 1;
	else if (a_sign == 0 && b_sign  > 0) oct_class = 2;
	else if (a_sign  < 0 && b_sign  > 0) oct_class = 3;
	else if (a_sign  < 0 && b_sign == 0) oct_class = 4;
	else if (a_sign  < 0 && b_sign  < 0) oct_class = 5;
	else if (a_sign == 0 && b_sign  < 0) oct_class = 6;
	else                                 oct_class = 7;
	
	//std::cout<<"oct class:"<<oct_class<<std::endl;
	
	return oct_class;
}

bool planar_comparator::compare_angle(mp_integer a, mp_integer b, mp_integer a_prime, mp_integer b_prime){
    
	//std::cout<<"compare angle:";
	//std::cout<<a<<b<<a_prime<<b_prime<<std::endl;

    int _oct_class = oct_class(a, b);
    int oct_class_prime = oct_class(a_prime, b_prime);

	//std::cout<<_oct_class<<std::endl;
	//std::cout<<oct_class_prime<<std::endl;
	

	if (_oct_class < oct_class_prime){
		//std::cout<<true<<std::endl;
		 return true;
	}
	else if (_oct_class > oct_class_prime){
		//std::cout<<false<<std::endl;
		return false;
	} 
	else {
		mp_integer product1 = a_prime * b;	
	    mp_integer product2 = a * b_prime;		
		if(debugging)
		std::cout<<"last clause"<<std::endl;
		//std::cout<<compare_to(product1,product2)<<std::endl;
	    return compare_to(product1,product2) < 0;
	}

}

bool planar_comparator::operator()(std::shared_ptr<inequality> ineq1, std::shared_ptr<inequality> ineq2) {

	

	std::shared_ptr<inequality> i1 = ineq1->widen(vars);
	std::shared_ptr<inequality> i2 = ineq2->widen(vars);


	if (std::dynamic_pointer_cast<dyadic_inequality>(i1)!=nullptr && std::dynamic_pointer_cast<dyadic_inequality>(i2)!=nullptr){

        std::shared_ptr<dyadic_inequality> d1  = std::dynamic_pointer_cast<dyadic_inequality>(i1);
	    std::shared_ptr<dyadic_inequality> d2  = std::dynamic_pointer_cast<dyadic_inequality>(i2);
		//std::cout<<"d1: " + d1->to_string()<<std::endl;
		//std::cout<<"d2: " + d2->to_string()<<std::endl;
	    return compare_angle(d1->a, d1->b, d2->a, d2->b);
	}
	else{
		
	   	 std::shared_ptr<unary_inequality> u1  = std::dynamic_pointer_cast<unary_inequality>(i1);
	     std::shared_ptr<unary_inequality> u2  = std::dynamic_pointer_cast<unary_inequality>(i2);
		 //std::cout<<"u1: " + u1->to_string()<<std::endl;
		 //std::cout<<"u2: " + u2->to_string()<<std::endl;
	    return compare_angle(u1->a, 0, u2->a, 0);
	}

}