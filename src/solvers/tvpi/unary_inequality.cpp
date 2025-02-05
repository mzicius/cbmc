#include "unary_inequality.h"
#include <iostream>

unary_inequality::unary_inequality(std::string _x, mp_integer _a, mp_integer _c)
: x(_x)
{
    mp_integer common_factor = gcd(abs(_a),abs(_c));
    a = _a/common_factor;
    c = _c/common_factor;
}

std::vector<std::string> unary_inequality::vars(){
    return {x};
}

int unary_inequality::sign_a(){
    return sign(a);
}

std::vector<coordinate> unary_inequality::intercepts(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max){
   
    std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(widen({x,y}));
	return d->intercepts(x, y, x_min, y_min, x_max, y_max);

}

std::vector<coordinate> unary_inequality::halfspace(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max){

    std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(widen({x,y}));
	return d->halfspace(x, y, x_min, y_min, x_max, y_max);

}

std::shared_ptr<inequality> unary_inequality::widen(std::vector<std::string> vars){

    
    if (vars.size() > 2){
	    return nullptr;
	}
        
    else if (vars.size() == 2 && x==vars[0]){
	    return  std::make_shared<dyadic_inequality>(vars[0], vars[1], a, 0, c);  
        
    }
	else if (vars.size() == 2 && x==vars[1]){
	    return std::make_shared<dyadic_inequality>(vars[0], vars[1], 0, a, c);  
    }
    else if (vars.size() == 1 && x==vars[0]){
        
        return std::make_shared<unary_inequality>(vars[0],a,c);
        
    }
    else
    {
	    return nullptr;
    }
  
}

std::string unary_inequality::to_string(){

    int a_sign = sign(a);
    int a_abs_one = compare_to(abs(a),1);

    std::string sb;
   
	if (a_abs_one == 0 && a_sign < 0) sb.append("-" + x);
	else if (a_abs_one == 0 && a_sign > 0) sb.append(x);
	else if (a_sign < 0) sb.append(integer2string(a,10) + x);
	else if (a_sign > 0) sb.append(integer2string(a,10) + x);

	sb.append("\u2264" + integer2string(c,10));

    return sb;
  
}

std::string unary_inequality::to_ltx_label(){
    
    int a_sign = sign(a);
    int a_abs_one = compare_to(abs(a),1);

    std::string sb;
   
	if (a_abs_one == 0 && a_sign < 0) sb.append("-" + x);
	else if (a_abs_one == 0 && a_sign > 0) sb.append(x);
	else if (a_sign < 0) sb.append(integer2string(a,10) + x);
	else if (a_sign > 0) sb.append(integer2string(a,10) + x);

	sb.append("\\leq" + integer2string(c,10));

    return sb;

}

std::shared_ptr<unary_inequality> cast_to_unary(std::shared_ptr<inequality> i){
  return std::dynamic_pointer_cast<unary_inequality>(i);
}