#include <iostream>
#include "hull.h"
//#include <matplotplusplus/matplot.h>

void hull::rm_excess(std::shared_ptr<inequality> ref, std::vector<std::shared_ptr<inequality>> &ls){


ls.erase(std::remove_if(ls.begin(),ls.end(),[ref](std::shared_ptr<inequality> i){return cmp_angle(ref,i)==inequality::GT;}),ls.end());


}

std::vector<std::shared_ptr<inequality>> hull::calc_hull(std::vector<std::shared_ptr<inequality>> xs, std::vector<std::shared_ptr<inequality>> ys){

std::cout<<"hull start function"<<std::endl;

/*
std::cout<<std::endl;
std::cout<<"xs: "<<std::endl;
print_cons(xs);
std::cout<<"ys: "<<std::endl;
print_cons(ys);
std::cout<<std::endl;
*/

std::vector<std::shared_ptr<inequality>>  res;

if (xs.empty() || ys.empty()) {
    return res;
}

std::shared_ptr<inequality> x = xs.front();
std::shared_ptr<inequality> y = ys.front();

std::shared_ptr<inequality> xn = xs.back();
std::shared_ptr<inequality> yn = ys.back();

/*
std::cout<<"xn: ";
show(xn);

std::cout<<"yn: ";
show(yn);
std::cout<<std::endl;
*/

//outerIsX = advanceOuter yn (ys++[y]) xn (xs++[x])
//outerIsY = advanceOuter xn (xs++[x]) yn (ys++[y])

ys.push_back(y);
xs.push_back(x);

std::optional<rationalt> dist_LT, dist_GT;

switch (cmp_angle(x,y)) {

 case inequality::EQ:
    if(calc_shift(x,y)>rationalt(0)){
        advance_outer(res,xn,xs,yn,ys);
    }
    else{
        advance_outer(res,yn,ys,xn,xs);
    }
    //rm_excess(x,res);
    break;
 case inequality::LT:
    dist_LT = calc_dist(yn,x,y);
    if(dist_LT.has_value() && dist_LT.value()<rationalt(0)){
        advance_outer(res,yn,ys,xn,xs);
    }
    else{
        advance_outer(res,xn,xs,yn,ys);
    }
    //rm_excess(x,res);
    break;
 case inequality::GT: 
    dist_GT = calc_dist(xn,y,x);
    if(dist_GT.has_value() && dist_GT.value()<rationalt(0)){
        advance_outer(res,xn,xs,yn,ys);
    }
    else{
        advance_outer(res,yn,ys,xn,xs);
    }
    //rm_excess(y,res);
    break;
}

//rm_syn_red(res);

/* syntax redundancies 
std::cout<<"vector before clean up"<<std::endl;
print_cons(res);
*/

//res = rm_local_red(res);

/* local redundancies 
std::cout<<"vector after local clean up"<<std::endl;
print_cons(res);
*/
 
std::cout<<"final res"<<std::endl;
print_cons(res);

return res;

}

void hull::join(std::string in_dir_xs, std::string in_dir_ys, std::string out_dir){

    std::cout<<in_dir_xs<<std::endl;
    std::cout<<in_dir_ys<<std::endl;

	std::vector<std::shared_ptr<inequality>> xs = parser::parse_from_csv(in_dir_xs);
    std::vector<std::shared_ptr<inequality>> ys = parser::parse_from_csv(in_dir_ys);

	std::vector<std::shared_ptr<inequality>> rs;

    rs = hull::calc_hull(xs,ys);

	std::ofstream output;

	output.open(out_dir);

	for(auto v:rs){
			
	if(std::dynamic_pointer_cast<unary_inequality>(v)!=nullptr){
		std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(v);
		output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";
	}
	else if(std::dynamic_pointer_cast<dyadic_inequality>(v)!=nullptr){
		std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(v);
		output<<d->x + ","+ d->y + "," + integer2string(d->a) + "," + integer2string(d->b) + ","+ integer2string(d->c)+ "\n";
	}

	}

	output.close();

}


