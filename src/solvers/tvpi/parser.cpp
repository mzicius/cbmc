#include "parser.h"
#include <iostream>

std::shared_ptr<inequality> parser::parse_inequality(std::string line){
        
        std::shared_ptr<inequality> c;
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string token;

        std::string l = ss.str();
        
        while(getline(ss,token,',')){
           parts.push_back(token);
        }
        
        if (parts.size()==2) c = std::make_shared<constant_inequality>(parts[0]<=parts[1]);
        if(parts.size()==3) c = inequality_factory::make_inequality(parts[0],string2integer(parts[1]),string2integer(parts[2]));
        if(parts.size()==5)  c = std::make_shared<dyadic_inequality>(parts[0],parts[1],string2integer(parts[2]),string2integer(parts[3]),string2integer(parts[4])); 
        
        /*else if(parts.size()==5){
                if(parts[2]=="0"){
                        inequality_factory::make_inequality(parts[1],string2integer(parts[3]),string2integer(parts[4]));
                }
                if(parts[3]=="0"){
                        inequality_factory::make_inequality(parts[0],string2integer(parts[2]),string2integer(parts[4]));
                }
                else{
                        c = std::make_shared<dyadic_inequality>(parts[0],parts[1],string2integer(parts[2]),string2integer(parts[3]),string2integer(parts[4]));  
                }
        
      
        }
        */


        return c;

}

std::vector<std::shared_ptr<inequality>> parser::parse_from_csv(std::string read_dir){

        auto loc = read_dir.find_last_of("/");
        auto out_dir = read_dir.substr(0,loc+1);
        auto file = read_dir.substr(loc+1);

        //std::cout<<read_dir<<std::endl;

        std::vector<std::shared_ptr<inequality>> cs;

        //std::cout<<"size in parser: "<<cs.size()<<std::endl;

        std::ifstream input_file(read_dir);
        std::string line; 

        while(getline(input_file,line)){
        
        if(line[0]!='#'){
                cs.push_back(parser::parse_inequality(line));
        }

        }
        
        //std::cout<<"cs in parser end: "<<cs.size()<<std::endl;

       return cs;
}

void parser::complete_system(std::string read_dir, std::string file,std::string dir){

std::string out_dir = dir;

std::vector<std::shared_ptr<inequality>> cs;
std::vector<std::shared_ptr<inequality>> vs;

std::ifstream input_file(read_dir);
std::string line; 

while(getline(input_file,line)){
        
        std::vector<std::string> parts;
        std::stringstream ss(line);
        std::string token;

        std::string l = ss.str();
        
        if (l.front()!='#'){
                //std::cout<<l<<std::endl;
                while(getline(ss,token,',')){
                        parts.push_back(token);
                }
        }

        if (parts.size()==2) cs.push_back(std::make_shared<constant_inequality>(parts[0]<=parts[1]));
        else if(parts.size()==3) cs.push_back(inequality_factory::make_inequality(parts[0],string2integer(parts[1]),string2integer(parts[2])));
        else if(parts.size()==5) cs.push_back(inequality_factory::make_inequality(parts[0],parts[1],string2integer(parts[2]),string2integer(parts[3]),string2integer(parts[4])));

}

std::ofstream output;

file = file.substr(0,file.find("."));

output.open(out_dir + file + ".out");

for(std::shared_ptr<inequality>c0:cs){

//std::cout<<"c0: " + c0->to_string()<<std::endl;
//std::cout<<"cs:";
//print_ins(cs);
vs = complete::inc_complete(vs,c0);
//std::cout<<"Inter vs:";
//print_ins(vs);
if (is_bottom(vs)) break;
//std::cout<<std::endl;
}
//std::cout<<"final vs: ";
//print_ins(vs);

for(auto v:vs){
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

