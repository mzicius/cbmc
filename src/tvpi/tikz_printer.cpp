#include "tikz_printer.h"
#include "unary_inequality.h"
#include "dyadic_inequality.h"
#include <iostream>
#include <fstream>
#include "inequality_factory.h"
void tikz_printer::print_tikz_system(std::vector<std::shared_ptr<inequality>> cs, std::string file_name, std::string color, int half_range){

std::cout<<cs.size()<<std::endl;
print_cons(cs);

//bool debug = true;
std::ofstream figure;
figure.open(file_name);
figure<<"\\documentclass{article}"<<std::endl; 
figure<<"\\usepackage{tikz}"<<std::endl;
figure<<"\\begin{document}"<<std::endl;
figure<<"\\begin{center}"<<std::endl;
figure<<"\\begin{tikzpicture}[scale=0.5]"<<std::endl;

//set of vars held by the system
std::set<std::string> vars = get_vars(cs);

//range of x and y axes 
//reasonable value to avoid empty pictures
mp_integer max_range = 8;
max_range = update_range(max_range,cs);
print_grid(figure,max_range,vars);

//seperate inequalities into groups

std::vector<std::shared_ptr<inequality>> unary_ineqs;
std::vector<std::shared_ptr<inequality>> dyadic_ineqs;
mp_integer a1, b1, c1, a2, b2, c2;
std::string x1, y1, x2, y2;

//seperate into groups
mp_integer range_start = -max_range;
mp_integer range_end = max_range;
mp_integer fixed_start = range_start;
mp_integer fixed_end = range_end;
std::string first_var = *vars.begin();
std::string second_var = *std::next(vars.begin());

for(std::shared_ptr<inequality> c:cs){

    if (std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
        std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);
        unary_ineqs.push_back(c);
    
    }
    else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
        std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
        std::shared_ptr<inequality> ineq = inequality_factory::make_inequality(d->x,d->y,d->a,d->b,d->c);
        if(ineq->arity()==1){
            unary_ineqs.push_back(ineq);
        }
        if(ineq->arity()==2){
            dyadic_ineqs.push_back(ineq);
        } 
        }

}

print_ins(unary_ineqs);

//plot inequalities 
for(std::shared_ptr<inequality> c:unary_ineqs){


    std::string down_shade = "\\shade[top color = red!70, bottom color = white,opacity=0.7] ";
    std::string up_shade = "\\shade[top color = white, bottom color = red!70,opacity=0.7] ";
    std::string right_shade = "\\shade[right color=red!70,left color=white,opacity=0.7] ";
    std::string left_shade = "\\shade[left color=red!70, right color=white,opacity=0.7] ";

	    std::shared_ptr<unary_inequality> u1  = std::dynamic_pointer_cast<unary_inequality>(c);
        a1 = u1->a;
        b1 = 0;
        c1 = u1->c;
        x1 = u1->x;
        y1 = "";
        rationalt h = rationalt(2)/rationalt(1);

        rationalt intercept = rationalt(c1)/rationalt(a1);
        if(x1 == first_var){

            figure<<"\\draw [thick, red] ("<<intercept<<","<<fixed_start<<") -- ("<<intercept<<","<<fixed_end<<") node[right, above] {$"<<u1->to_ltx_label()<<"$};"<<std::endl;
            //half-space

            if(a1.is_negative()){
                //face right
                std::cout<<"right"<<std::endl;
                figure<<left_shade<<"("<<intercept+h<<","<<fixed_end<<")-- ("<<intercept+h<<","<<fixed_start<<") -- ("<<intercept<<","<<fixed_start<<") -- ("<<intercept<<","<<fixed_end<<");"<<std::endl;
            }
            else{
                //face left
                std::cout<<"left"<<std::endl;
                std::cout<<intercept<<std::endl;
                figure<<right_shade<<"("<<intercept-h<<","<<fixed_end<<") -- ("<<intercept-h<<","<<fixed_start<<") -- ("<<intercept<<","<<fixed_start<<") -- ("<<intercept<<","<<fixed_end<<");"<<std::endl;
     
            }
        }
        
        else if(x1 == second_var){

    
            figure<<"\\draw [thick, red] ("<<fixed_start<<","<<intercept<<") -- ("<<fixed_end<<","<<intercept<<") node[left, above] {$"<<u1->to_ltx_label()<<"$};"<<std::endl;
            //half-space
            std::cout<<intercept<<std::endl;
                if(a1.is_negative()){
                //face up
                figure<<up_shade<<"("<<fixed_start<<","<<intercept<<") -- ("<<fixed_start<<","<<intercept+h<<") -- ("<<fixed_end<<","<<intercept+h<<")-- ("<<fixed_end<<","<<intercept<<");"<<std::endl;
            }
            else{
                //face down
                figure<<down_shade<<"("<<fixed_start<<","<<intercept<<") -- ("<<fixed_start<<","<<intercept-h<<") -- ("<<fixed_end<<","<<intercept-h<<")-- ("<<fixed_end<<","<<intercept<<");"<<std::endl;
            }
            
        }

}   
for(std::shared_ptr<inequality> c:dyadic_ineqs){
		std::shared_ptr<dyadic_inequality> d1  = std::dynamic_pointer_cast<dyadic_inequality>(c);
	    a1 = d1->a;
	    b1 = d1->b;
	    c1 = d1->c;
	    x1 = d1->x;
	    y1 = d1->y;

        std::string part;

        rationalt slope = (-rationalt(a1))/rationalt(b1);
        //rationalt intercept_two = rationalt(c1)/rationalt(b1);
        std::cout<<d1->to_string()<<std::endl;

        range_start = fixed_start;

        int rr = half_range;

        auto halfspace = d1->halfspace(d1->x,d1->y,-rr,-rr,rr,rr);
        std::string dy_fill = "\\fill[fill=blue, opacity=0.4] ";

        figure<<dy_fill<<"(" << halfspace[0].x << ", " << halfspace[0].y << ") -- (" << halfspace[1].x << ", " << halfspace[1].y << ") -- (" << halfspace[2].x << ", " << halfspace[2].y << ") -- (" << halfspace[3].x << ", " << halfspace[3].y << ")" << ";" << std::endl;

        if(slope.is_negative()){

            figure<<"\\draw [thick, blue] ("<< halfspace[2].x << ", " << halfspace[2].y << ") -- (" << halfspace[3].x << ", " << halfspace[3].y << ") node[right, above] {\\footnotesize $"<<d1->to_ltx_label()<<"$};"<<std::endl;
         
        }
        else{
            figure<<"\\draw [thick, blue] ("<< halfspace[0].x << ", " << halfspace[0].y << ") -- (" << halfspace[1].x << ", " << halfspace[1].y << ") node[right, above] {\\footnotesize $"<<d1->to_ltx_label()<<"$};"<<std::endl;
        
        }


       
}


figure<<"\\end{tikzpicture}"<<std::endl;
figure<<"\\end{center}"<<std::endl;
figure<<"\\end{document}"<<std::endl;

figure.close();
}

void tikz_printer::print_grid(std::ofstream& figure, mp_integer max_range,std::set<std::string> vars){

//update the length of axes so they end up after intersection points
mp_integer padding = 1;
max_range = max_range + padding;

mp_integer range_start = -max_range;
mp_integer range_end = max_range;


std::string first_var = *vars.begin();
std::string second_var = *std::next(vars.begin());

//draw axes
figure<<"\\draw[->] "<<"("<<range_start<<",0) -- ("<<range_end<<",0) node[right] {$"<<first_var<<"$};"<<std::endl;
figure<<"\\draw[->] "<<"(0,"<<range_start<<") -- (0,"<<range_end<<") node[above] {$"<<second_var<<"$};"<<std::endl;

//draw grid
mp_integer fixed_start = range_start;
mp_integer fixed_end = range_end;
range_start = range_start+1;

//dashes and numbers for vertical axis
while(range_start!=range_end){
    figure<<"\\draw[dashed] ("<<fixed_start<<","<<range_start<<") -- ("<<fixed_end<<","<<range_start<<");"<<std::endl;
    if(range_start!=0){
        figure<<"\\draw[] (0,"<<range_start<<") node [left] {\\footnotesize $"<<range_start<<"$};"<<std::endl;
    }
    range_start = range_start+1;
}

//dashes and numbers for horizonatal lines
range_start = fixed_start +1; 
while(range_start!=range_end){
    figure<<"\\draw[dashed] ("<<range_start<<","<<fixed_start<<") -- ("<<range_start<<","<<fixed_end<<");"<<std::endl;
    if(range_start!=0){
        figure<<"\\draw[] ("<<range_start<<", 0) node [below] {\\footnotesize $"<<range_start<<"$};"<<std::endl;
    }
    range_start = range_start+1;
    
}

}

mp_integer tikz_printer::update_range(mp_integer max_range,std::vector<std::shared_ptr<inequality>> cs){

    mp_integer range = max_range;
    for(std::shared_ptr<inequality> c:cs){

    mp_integer a1, b1, c1, a2, b2, c2;
	std::string x1, y1, x2, y2;

    if (std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
	    std::shared_ptr<unary_inequality> u1  = std::dynamic_pointer_cast<unary_inequality>(c);
        a1 = u1->a;
        b1 = 0;
        c1 = u1->c;
        x1 = u1->x;
        y1 = "";

        //verify the range of graph axes
        if(a1!=0){
            mp_integer intercept = abs(c1)/abs(a1);
            if(intercept>range){
                range = intercept;
            }
        }
	}
	else
	{
		std::shared_ptr<dyadic_inequality> d1  = std::dynamic_pointer_cast<dyadic_inequality>(c);
	    a1 = d1->a;
	    b1 = d1->b;
	    c1 = d1->c;
	    x1 = d1->x;
	    y1 = d1->y;

        //verify the range of graph axes
        if(a1!=0){
            mp_integer intercept_one = abs(c1)/abs(a1);
            if(intercept_one>range){
                range = intercept_one;
            }
        }
        if(b1!=0){
            mp_integer intercept_two = abs(c1)/abs(b1);
            if(intercept_two>range){
                range = intercept_two;
            }
        }
	}
    }
    return range;
}

std::set<std::string> tikz_printer::get_vars(std::vector<std::shared_ptr<inequality>> cs){

std::set<std::string> vars;
for(std::shared_ptr<inequality> c:cs){

    if(vars.size()<2){
        std::vector<std::string> vars_inside = c->vars();
        vars.insert(vars_inside.begin(),vars_inside.end());
    }

}

return vars;

}