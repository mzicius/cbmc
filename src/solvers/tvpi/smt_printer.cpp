#include "smt_printer.h"
#include <iostream>
#include <fstream>
#include <vector> 
#include <set>
#include "parser.h"

std::string smt_printer::sign_label(mp_integer m){
    if(m>=0) return "+";
    else return "-";
}

mp_integer  smt_printer::abs_number(mp_integer m){
    if (m.is_positive()) return m;
    else return m.negate();
}

std::string smt_printer::label(std::string s, mp_integer i){
    std::string label;
    std::string sign = sign_label(i);
    if(sign=="-"){
        std::cout<<"yes"<<std::endl;
        if(i<-1){
            label = label + "(* (- "+integer2string(abs(i))+") "+s+")";
        }
        else{
            label = label +"(- "+s+")";
        }            
    }
    else{
        if(i>1){
            label = label + "(* "+integer2string(i)+" "+s+")";
        }
        else{
            label = label +s;
        }
    }
    return label;
}

void smt_printer::print_system(std::string in_dir,std::string out_dir){

std::ofstream sat;
std::vector<std::shared_ptr<inequality>> cs = parser::parse_from_csv(in_dir);
print_cons(cs);
std::set<std::string> vars;
sat.open(out_dir);

for(auto c: cs){

    std::vector<std::string> vars_inside = c->vars();
    vars.insert(vars_inside.begin(),vars_inside.end());

}

sat<<"(set-logic QF_LRA)"<<std::endl;
sat<<"(set-option :produce-models true)"<<std::endl;
sat<<std::endl;

for (auto it = vars.begin(); it != vars.end(); ++it) {
        sat<<"(declare-fun "<<*it<<" () Real)"<<std::endl;
}
sat<<std::endl;



for(auto c:cs){

    if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
            std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);

            std::string label;
            std::string sign = sign_label(u->a);
            std::cout<<sign<<std::endl;

            if(sign=="-"){
                std::cout<<"yes"<<std::endl;
                if(u->a<-1){
                    label = label + "(* (- "+integer2string(abs(u->a))+") "+u->x+")";
                }
                else{
                    label = label +"(- "+u->x+")";
                }
                
            }
            else{
                if(u->a>1){
                    label = label + "(* "+integer2string(u->a)+" "+u->x+")";
                }
                else{
                    label = label +u->x;
                }
            }

            //std::cout<<label<<std::endl;
            std::string c_label;
            if(u->c<0){
                c_label ="(- "+integer2string(abs(u->c))+")";
            }
            else{
                c_label = integer2string(u->c);
            }
            sat<<"(assert ("<<"<= "<<label<<" "<<c_label<<"))"<<std::endl;

            //output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";


    }
    else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
            std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
            std::string c_label;
            if(d->c<0){
                c_label ="(- "+integer2string(abs(d->c))+")";
            }
            else{
                c_label = integer2string(d->c);
            }

            sat<<"(assert ("<<"<= (+ "<<label(d->x,d->a)<<" "<<label(d->y,d->b)<<") "<<c_label<<"))"<<std::endl;
    
    }

}

sat<<std::endl;
sat<<"(check-sat)"<<std::endl;
sat<<"(get-model)"<<std::endl;

sat.close();



}

void smt_printer::print_smt_entailment(){
    std::cout<<"entailment"<<std::endl;
}

void smt_printer::print_smt_closure(std::string in_file_one, std::string in_file_two, std::string smt_out){
 
 std::vector<std::shared_ptr<inequality>> sys1 = parser::parse_from_csv(in_file_one);
 std::vector<std::shared_ptr<inequality>> sys2 = parser::parse_from_csv(in_file_two);
 std::ofstream output;

 output.open(smt_out);
 
 //preq 
 output<<"(set-logic QF_LRA)"<<"\n";
 output<<"(set-option :produce-models true)"<<"\n";
 output<<"\n";

//capture variables of the first system
std::set<std::string> vars1;
for(std::shared_ptr<inequality> i: sys1){
 std::vector<std::string> hold = i->vars();
 vars1.insert(hold.begin(),hold.end());
}

//capture variables of the second system
std::set<std::string> vars2;
for(std::shared_ptr<inequality> i: sys2){
 std::vector<std::string> hold = i->vars();
 vars2.insert(hold.begin(),hold.end());  
}

//capture variables found in both sets 
std::set<std::string> vars_all;
vars_all.insert(vars1.begin(),vars1.end());
vars_all.insert(vars2.end(),vars2.end());

//functions
for(auto it=vars_all.begin();it!=vars_all.end();++it){
 output<<"(declare-fun "<<*it<<" () Real)"<<"\n";
}
output<<"\n";

//system a 
output<<"(define-fun in_system_a (";
for(auto its=vars1.begin(); its!=vars1.end();++its){

    if(its == vars1.begin())
    {
        output << "(" << *its << " Real)";
    }
    else{
        output << " (" << *its << " Real)";
    }
}

 output<<") Bool"<<"\n";
 output<<"(and ";
 for(auto c:sys1){
 if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
            std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);

            std::string label;
            std::string sign = sign_label(u->a);
            std::cout<<sign<<std::endl;

            if(sign=="-"){
                std::cout<<"yes"<<std::endl;
                if(u->a<-1){
                    label = label + "(* (- "+integer2string(abs(u->a))+") "+u->x+")";
                }
                else{
                    label = label +"(- "+u->x+")";
                }
                
            }
            else{
                if(u->a>1){
                    label = label + "(* "+integer2string(u->a)+" "+u->x+")";
                }
                else{
                    label = label +u->x;
                }
            }

            //std::cout<<label<<std::endl;
            std::string c_label;
            if(u->c<0){
                c_label ="(- "+integer2string(abs(u->c))+")";
            }
            else{
                c_label = integer2string(u->c);
            }
            output<<"("<<"<= "<<label<<" "<<c_label<<")"<<std::endl;

            //output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";


    }
 else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
            std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
            std::string c_label;
            if(d->c<0){
                c_label ="(- "+integer2string(abs(d->c))+")";
            }
            else{
                c_label = integer2string(d->c);
            }

            output<<"("<<"<= (+ "<<label(d->x,d->a)<<" "<<label(d->y,d->b)<<") "<<c_label<<")"<<std::endl;
    
    }
else if(std::dynamic_pointer_cast<constant_inequality>(c)!=nullptr){
    std::shared_ptr<constant_inequality> ci = std::dynamic_pointer_cast<constant_inequality>(c);
    if(ci->sat){
        output<<"true"<<std::endl;
    }
    else{
        output<<"false"<<std::endl;
    }
}
 }
 
 output<<"))"<<"\n";
 output<<"\n";

//system b
bool flag = true; 
output<<"(define-fun in_system_b (";

//check if system b has variables (unsat check)
if(vars2.empty()){
    flag = false;
    vars2 = vars1;
}
std::cout<<"printer flag: "<<flag<<std::endl;
for(auto i=vars2.begin(); i!=vars2.end();++i){

    if(i == vars2.begin())
    {
        output << "(" << *i << " Real)";
    }
    else{
        output << " (" << *i << " Real)";
    }
}
output<<") Bool"<<"\n";

if(flag){
output<<"(and ";
}

for(auto c:sys2){

    if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
            std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);

            std::string label;
            std::string sign = sign_label(u->a);
            std::cout<<sign<<std::endl;

            if(sign=="-"){
                std::cout<<"yes"<<std::endl;
                if(u->a<-1){
                    label = label + "(* (- "+integer2string(abs(u->a))+") "+u->x+")";
                }
                else{
                    label = label +"(- "+u->x+")";
                }
                
            }
            else{
                if(u->a>1){
                    label = label + "(* "+integer2string(u->a)+" "+u->x+")";
                }
                else{
                    label = label +u->x;
                }
            }

            //std::cout<<label<<std::endl;
            std::string c_label;
            if(u->c<0){
                c_label ="(- "+integer2string(abs(u->c))+")";
            }
            else{
                c_label = integer2string(u->c);
            }
            output<<"("<<"<= "<<label<<" "<<c_label<<")"<<std::endl;

            //output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";


    }
    else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
            std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
            std::string c_label;
            if(d->c<0){
                c_label ="(- "+integer2string(abs(d->c))+")";
            }
            else{
                c_label = integer2string(d->c);
            }

            output<<"("<<"<= (+ "<<label(d->x,d->a)<<" "<<label(d->y,d->b)<<") "<<c_label<<")"<<std::endl;
    
    }
    else if(std::dynamic_pointer_cast<constant_inequality>(c)!=nullptr){
    std::shared_ptr<constant_inequality> ci = std::dynamic_pointer_cast<constant_inequality>(c);
    if(ci->sat){
        output<<"true"<<std::endl;
    }
    else{
        output<<"false"<<std::endl;
    }
    
}

}

if(flag){
    output<<"))";
}
else{
     output<<")";
}

output<<"\n";
output<<"\n";

//check if there is a vertice in system a that is not in system b and vice versa 

output<<"(assert (or (and (in_system_a ";
auto vit = vars_all.begin();
while(std::next(vit)!=vars_all.end()){
    output<<*vit<<" ";
    vit++;
}
output<<*vit<<") (not (in_system_b ";
vit = vars_all.begin();
while(std::next(vit)!=vars_all.end()){
    output<<*vit<<" ";
    vit++;
}
output<<*vit<<"))) (and (not (in_system_a ";
vit = vars_all.begin();
while(std::next(vit)!=vars_all.end()){
    output<<*vit<<" ";
    vit++;
}
output<<*vit<<")) (in_system_b ";
vit = vars_all.begin();
while(std::next(vit)!=vars_all.end()){
    output<<*vit<<" ";
    vit++;
}
output<<*vit;


output<<"))))"<<"\n";
output<<"\n";   
output<<"\n";
output<<"(check-sat)"<<"\n";
output<<"(get-model)"<<"\n";

output.close();
}

void smt_printer::print_hull(std::string in_one,std::string in_two, std::string in_three, std::string smt_out){
    std::vector<std::shared_ptr<inequality>> sys1 = parser::parse_from_csv(in_one);
    std::vector<std::shared_ptr<inequality>> sys2 = parser::parse_from_csv(in_two);
    std::vector<std::shared_ptr<inequality>> sys3 = parser::parse_from_csv(in_three);
    std::ofstream output;
    output.open(smt_out);

     //preq 
    output<<"(set-logic QF_LRA)"<<"\n";
    output<<"(set-option :produce-models true)"<<"\n";
    output<<"\n";

    //capture variables of the first system
    std::set<std::string> vars1;
    for(std::shared_ptr<inequality> i: sys1){
    std::vector<std::string> hold = i->vars();
    vars1.insert(hold.begin(),hold.end());
    }

    //capture variables of the second system
    std::set<std::string> vars2;
    for(std::shared_ptr<inequality> i: sys2){
    std::vector<std::string> hold = i->vars();
    vars2.insert(hold.begin(),hold.end());  
    }

    //capture variables of the third system
    std::set<std::string> vars3;
    for(std::shared_ptr<inequality> i: sys3){
    std::vector<std::string> hold = i->vars();
    vars3.insert(hold.begin(),hold.end());  
    }

    //capture variables found in both sets 
    std::set<std::string> vars_all;
    vars_all.insert(vars1.begin(),vars1.end());
    vars_all.insert(vars2.begin(),vars2.end());
    vars_all.insert(vars3.begin(),vars3.end());

    //functions
    for(auto it=vars_all.begin();it!=vars_all.end();++it){
    output<<"(declare-fun "<<*it<<" () Real)"<<"\n";
    }
    output<<"\n";

//system a 
output<<"(define-fun in_system_a (";
for(auto its=vars1.begin(); its!=vars1.end();++its){

    if(its == vars1.begin())
    {
        output << "(" << *its << " Real)";
    }
    else{
        output << " (" << *its << " Real)";
    }
}

output<<") Bool"<<"\n";
output<<"(and ";
for(auto c:sys1){
 if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
            std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);

            std::string label;
            std::string sign = sign_label(u->a);
            std::cout<<sign<<std::endl;

            if(sign=="-"){
                std::cout<<"yes"<<std::endl;
                if(u->a<-1){
                    label = label + "(* (- "+integer2string(abs(u->a))+") "+u->x+")";
                }
                else{
                    label = label +"(- "+u->x+")";
                }
                
            }
            else{
                if(u->a>1){
                    label = label + "(* "+integer2string(u->a)+" "+u->x+")";
                }
                else{
                    label = label +u->x;
                }
            }

            //std::cout<<label<<std::endl;
            std::string c_label;
            if(u->c<0){
                c_label ="(- "+integer2string(abs(u->c))+")";
            }
            else{
                c_label = integer2string(u->c);
            }
            output<<"("<<"<= "<<label<<" "<<c_label<<")"<<std::endl;

            //output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";


    }
 else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
            std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
            std::string c_label;
            if(d->c<0){
                c_label ="(- "+integer2string(abs(d->c))+")";
            }
            else{
                c_label = integer2string(d->c);
            }

            output<<"("<<"<= (+ "<<label(d->x,d->a)<<" "<<label(d->y,d->b)<<") "<<c_label<<")"<<std::endl;
    
    }
else if(std::dynamic_pointer_cast<constant_inequality>(c)!=nullptr){
    std::shared_ptr<constant_inequality> ci = std::dynamic_pointer_cast<constant_inequality>(c);
    if(ci->sat){
        output<<"true"<<std::endl;
    }
    else{
        output<<"false"<<std::endl;
    }
}
 }
 
output<<"))"<<"\n";
output<<"\n";

//system b
output<<"(define-fun in_system_b (";
for(auto its=vars2.begin(); its!=vars2.end();++its){

    if(its == vars2.begin())
    {
        output << "(" << *its << " Real)";
    }
    else{
        output << " (" << *its << " Real)";
    }
}

output<<") Bool"<<"\n";
output<<"(and ";
for(auto c:sys2){
 if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
            std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);

            std::string label;
            std::string sign = sign_label(u->a);
            std::cout<<sign<<std::endl;

            if(sign=="-"){
                std::cout<<"yes"<<std::endl;
                if(u->a<-1){
                    label = label + "(* (- "+integer2string(abs(u->a))+") "+u->x+")";
                }
                else{
                    label = label +"(- "+u->x+")";
                }
                
            }
            else{
                if(u->a>1){
                    label = label + "(* "+integer2string(u->a)+" "+u->x+")";
                }
                else{
                    label = label +u->x;
                }
            }

            //std::cout<<label<<std::endl;
            std::string c_label;
            if(u->c<0){
                c_label ="(- "+integer2string(abs(u->c))+")";
            }
            else{
                c_label = integer2string(u->c);
            }
            output<<"("<<"<= "<<label<<" "<<c_label<<")"<<std::endl;

            //output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";


    }
 else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
            std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
            std::string c_label;
            if(d->c<0){
                c_label ="(- "+integer2string(abs(d->c))+")";
            }
            else{
                c_label = integer2string(d->c);
            }

            output<<"("<<"<= (+ "<<label(d->x,d->a)<<" "<<label(d->y,d->b)<<") "<<c_label<<")"<<std::endl;
    
    }
else if(std::dynamic_pointer_cast<constant_inequality>(c)!=nullptr){
    std::shared_ptr<constant_inequality> ci = std::dynamic_pointer_cast<constant_inequality>(c);
    if(ci->sat){
        output<<"true"<<std::endl;
    }
    else{
        output<<"false"<<std::endl;
    }
}
 }
 
output<<"))"<<"\n";
output<<"\n";

//system c
output<<"(define-fun in_system_c (";
if(vars3.empty()){
    for(auto its=vars_all.begin(); its!=vars_all.end();++its){

        if(its == vars_all.begin())
        {
            output << "(" << *its << " Real)";
        }
        else{
            output << " (" << *its << " Real)";
        }
    }   
}
for(auto its=vars3.begin(); its!=vars3.end();++its){

    if(its == vars3.begin())
    {
        output << "(" << *its << " Real)";
    }
    else{
        output << " (" << *its << " Real)";
    }
}

output<<") Bool"<<"\n";
if(!sys3.empty()){
    output<<"(and ";
}

for(auto c:sys3){
 if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
            std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);

            std::string label;
            std::string sign = sign_label(u->a);
            std::cout<<sign<<std::endl;

            if(sign=="-"){
                std::cout<<"yes"<<std::endl;
                if(u->a<-1){
                    label = label + "(* (- "+integer2string(abs(u->a))+") "+u->x+")";
                }
                else{
                    label = label +"(- "+u->x+")";
                }
                
            }
            else{
                if(u->a>1){
                    label = label + "(* "+integer2string(u->a)+" "+u->x+")";
                }
                else{
                    label = label +u->x;
                }
            }

            //std::cout<<label<<std::endl;
            std::string c_label;
            if(u->c<0){
                c_label ="(- "+integer2string(abs(u->c))+")";
            }
            else{
                c_label = integer2string(u->c);
            }
            output<<"("<<"<= "<<label<<" "<<c_label<<")"<<std::endl;

            //output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";


    }
 else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
            std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
            std::string c_label;
            if(d->c<0){
                c_label ="(- "+integer2string(abs(d->c))+")";
            }
            else{
                c_label = integer2string(d->c);
            }

            output<<"("<<"<= (+ "<<label(d->x,d->a)<<" "<<label(d->y,d->b)<<") "<<c_label<<")"<<std::endl;
    
    }
else if(std::dynamic_pointer_cast<constant_inequality>(c)!=nullptr){
    std::shared_ptr<constant_inequality> ci = std::dynamic_pointer_cast<constant_inequality>(c);
    if(ci->sat){
        output<<"true"<<std::endl;
    }
    else{
        output<<"false"<<std::endl;
    }
}
 }

if(sys3.empty()){
    output<<"true"<<std::endl;
    output<<")"<<"\n";
}
else{
    output<<"))"<<"\n";
}

output<<"\n";


/*
(assert (=> (and (in_system_a a b) (in_system_b a b))
             (in_system_c a b)))
*/
output<<"(assert ";
output<<"(=> ";
output<<"(or ";
output<<"(in_system_a ";
for(auto its=vars1.begin(); its!=vars1.end();++its){

    if(its == vars1.begin())
    {
        output << *its;
    }
    else{
        output <<" "<< *its;
    }
}
output<<") (in_system_b ";
for(auto its=vars2.begin(); its!=vars2.end();++its){

    if(its == vars2.begin())
    {
        output << *its;
    }
    else{
        output <<" "<< *its;
    }
}
output<<")) ";
output<<"(in_system_c ";
if(sys3.empty()){
    for(auto its=vars_all.begin(); its!=vars_all.end();++its){

    if(its == vars_all.begin())
    {
        output << *its;
    }
    else{
        output <<" "<< *its;
    }
}
}
for(auto its=vars3.begin(); its!=vars3.end();++its){

    if(its == vars3.begin())
    {
        output << *its;
    }
    else{
        output <<" "<< *its;
    }
}

output<<")))"<<std::endl;

output<<"(check-sat)"<<"\n";
output<<"(get-model)"<<"\n";
output.close();
}