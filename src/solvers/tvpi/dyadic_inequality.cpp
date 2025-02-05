#include "dyadic_inequality.h"
#include <iostream>

dyadic_inequality::dyadic_inequality(std::string _x, std::string _y, mp_integer _a, mp_integer _b, mp_integer _c)
: x(_x),
  y(_y)
{  
	mp_integer common_factor = gcd(abs(_a),gcd(abs(_b),abs(_c)));
	//std::cout<<"factor: " + integer2string(common_factor)<<std::endl;
	a = _a/common_factor;
	b = _b/common_factor;
	c = _c/common_factor;
}

std::vector<std::string> dyadic_inequality::vars(){
    return {x,y};
}

int dyadic_inequality::sign_a(){
    return sign(a);
}

int dyadic_inequality::sign_b(){
    return sign(b);
}

std::vector<coordinate> dyadic_inequality::intercepts(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max){
    
    long double  ad = a.to_long();
    long double  bd = b.to_long();
    long double  cd = c.to_long();
    
	if (this->x==y && this->y==x){
	    bd = c.to_long();
	    cd = b.to_long();		
	}
	
	/*
	std::cout<<ad<<std::endl;
	std::cout<<bd<<std::endl;
	std::cout<<cd<<std::endl;
	*/

	std::vector<coordinate> intercepts;

    /*
    std::cout<<"ad: " + std::to_string(ad)<<std::endl;
    std::cout<<"bd: " + std::to_string(bd)<<std::endl;
    std::cout<<"cd: " + std::to_string(cd)<<std::endl;
    */
    
    
    if (sign_a() != 0){

        
	    long double x_at_y_min = (cd - bd * y_min) / ad;
	    long double x_at_y_max =((cd - bd * y_max) / ad);

        
        
            std::cout<< "xmin:" + std::to_string(x_min)<<std::endl;
            std::cout<< "xmax:" + std::to_string(x_max)<<std::endl;
            std::cout<< "ymin:" + std::to_string(y_min)<<std::endl;
            std::cout<< "ymax:" + std::to_string(y_max)<<std::endl;
            std::cout<< "x_at_y_min:" + std::to_string(x_at_y_min)<<std::endl;
            std::cout<< "x_at_y_max:" + std::to_string(x_at_y_max)<<std::endl;
        
   
	   if (x_min <= x_at_y_min && x_at_y_min <= x_max) intercepts.push_back(coordinate(x_at_y_min, y_min));	
	   if (x_min <= x_at_y_max && x_at_y_max <= x_max) intercepts.push_back(coordinate(x_at_y_max, y_max));	
        
    }

	if (sign_b() != 0)
	{   

	    long double y_at_x_min = (cd - ad * x_min) / bd;
	    long double y_at_x_max = (cd - ad * x_max) / bd;

        
        std::cout<< "y_at_x_min:" + std::to_string(y_at_x_min)<<std::endl;
        std::cout<< "y_at_x_max" +  std::to_string(y_at_x_max)<<std::endl;
        
      
	    if (y_min <= y_at_x_min && y_at_x_min <= y_max) intercepts.push_back(coordinate(x_min, y_at_x_min));	
	    if (y_min <= y_at_x_max && y_at_x_max <= y_max) intercepts.push_back(coordinate(x_max, y_at_x_max));	
	}
		
	return intercepts;
}

std::vector<coordinate> dyadic_inequality::halfspace(std::string x, std::string y, int x_min, int y_min, int x_max, int y_max){
    	
        std::vector<coordinate> intercept = intercepts(x, y, x_min, y_min, x_max, y_max);
		//std::stable_sort(intercept.begin(),intercept.end(), comp);

		std::vector<coordinate> polygon;

		coordinate c1 (intercept.front());
		coordinate c2 = (intercept.back());

		double x1 = c1.x;
		double y1 = c1.y;
		double x2 = c2.x;
		double y2 = c2.y;

		double delta_x = x2 - x1;
		double delta_y = y2 - y1;

		double stroke_width = 1;
		double x_stroke_width, y_stroke_width;

		if (delta_x == 0)
		{
			x_stroke_width = stroke_width;
			y_stroke_width = 0;
		}
		else
		{
			double theta = atan(abs(delta_y) / abs(delta_x));
			x_stroke_width = stroke_width * sin(theta);
			y_stroke_width = stroke_width * cos(theta);
		}

		if (sign(a) == -1) x_stroke_width = -x_stroke_width;
		if (sign(b) == -1) y_stroke_width = -y_stroke_width;

		polygon.push_back(coordinate(x1, y1));
		polygon.push_back(coordinate(x2, y2));
		polygon.push_back(coordinate(x2 - x_stroke_width, y2 - y_stroke_width));
		polygon.push_back(coordinate(x1 - x_stroke_width, y1 - y_stroke_width));

		return polygon;
}

std::shared_ptr<inequality> dyadic_inequality::widen(std::vector<std::string> vars){
    	
    if (vars.size() > 2){
		return nullptr;
	}
	else if (vars.size() == 2 && x==vars[0] && y==vars[1]){
	    return std::make_shared<dyadic_inequality>(vars[0],vars[1],a,b,c);
	}
	else{
		return nullptr;
	}
}

std::string dyadic_inequality::to_string(){
    
    int a_sign = sign_a();
    int b_sign = sign_b();

	bool a_abs_one = compare_to(abs(a),1) == 0;
    bool b_abs_one = compare_to(abs(b),1) == 0;

	std::string sb;

	if (a_abs_one && a_sign < 0) sb.append("-" + x);
	else if (a_abs_one && a_sign > 0) sb.append(x);
	else if (a_sign < 0) sb.append(integer2string(a) + x);
	else if (a_sign > 0) sb.append(integer2string(a) + x);

	if (b_abs_one && b_sign < 0) sb.append("-" + y);
	else if (b_abs_one && b_sign > 0) sb.append("+" + y);
	else if (b_sign < 0) sb.append(integer2string(b) + y);
	else if (b_sign > 0) sb.append("+" + integer2string(b) + y);

	sb.append("\u2264" + integer2string(c));

	return sb; 
      
}

std::string dyadic_inequality::to_ltx_label(){
    
	int a_sign = sign_a();
    int b_sign = sign_b();

	bool a_abs_one = compare_to(abs(a),1) == 0;
    bool b_abs_one = compare_to(abs(b),1) == 0;

	std::string sb;

	if (a_abs_one && a_sign < 0) sb.append("-" + x);
	else if (a_abs_one && a_sign > 0) sb.append(x);
	else if (a_sign < 0) sb.append(integer2string(a) + x);
	else if (a_sign > 0) sb.append(integer2string(a) + x);

	if (b_abs_one && b_sign < 0) sb.append("-" + y);
	else if (b_abs_one && b_sign > 0) sb.append("+" + y);
	else if (b_sign < 0) sb.append(integer2string(b) + y);
	else if (b_sign > 0) sb.append("+" + integer2string(b) + y);

	sb.append("\\leq" + integer2string(c));

	return sb; 
}

std::shared_ptr<dyadic_inequality> cast_to_dyadic(std::shared_ptr<inequality> i){
  return std::dynamic_pointer_cast<dyadic_inequality>(i);
}