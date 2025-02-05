#include "complete.h"
#include "parser.h"
#include "vertex.h"
#include <string>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include "../../analyses/tvpi/tvpi_domaint.h"
#include <fstream>
#include <map>

std::ofstream complete::detailed_stats;
int complete::intermediate_result_call;
std::shared_ptr<inequality> complete::in_result;

bool const complete::debugging = false;

int complete::modulo(int i, int l){
	int m = i % l;
	if (m < 0) m = m + l;
	return m;
}

void complete::closure(std::string in_dir, std::string out_dir){

	//changed
	//tvpi_domaint::result_call = 0; 

	//std::cout<<in_dir<<std::endl;

	std::vector<std::shared_ptr<inequality>> cs;
	//std::cout<<cs.size()<<std::endl;
	cs = parser::parse_from_csv(in_dir);

	std::vector<std::shared_ptr<inequality>> vs;


	std::ofstream output;
	output.open(out_dir);
	//to fix
	//tvpi_domaint::input_sys_size = cs.size();

	//detailed stats
    detailed_stats.open("../../test/detailed_stats.txt",std::ios_base::app);
	detailed_stats<< "input system: "<<in_dir<<std::endl;
	detailed_stats<< "output system size: "<<out_dir<<std::endl;

	//inputs vs no. current size of system (after result)
	std::ofstream plot_inputs_results;
	std::string version_label;
	std::string dim_label;
    std::string con_label;
	std::string full_label = in_dir;
	full_label = full_label.substr(full_label.find_last_of('/'));
	//std::cout<<"full label: "<<full_label<<std::endl;
	size_t last_fslash_pos = full_label.find_last_of('/');
	size_t first_dash_pos = full_label.find_first_of('-');
	size_t second_dash_pos = full_label.find('-',first_dash_pos+1);
	auto v_pos = full_label.find('v');
	auto ex_pos = full_label.find(".in");
	version_label = full_label.substr(v_pos,ex_pos-v_pos);

	dim_label = full_label.substr(last_fslash_pos+1,first_dash_pos-1);
    con_label = full_label.substr(first_dash_pos + 1, second_dash_pos - first_dash_pos - 1);

	plot_inputs_results.open("../../plots/"+dim_label+"/"+dim_label+"D-IR.txt",std::ios_base::app);

	size_t item_id = 1;
	for(std::shared_ptr<inequality>c0:cs){


	//calculation of completion
	vs = complete::inc_complete(vs,c0);
	//to fix
	//tvpi_domaint::output_sys_size = vs.size();

	plot_inputs_results<<version_label<<","; 
	plot_inputs_results<<item_id<<",";
	//to fix
	//plot_inputs_results<<tvpi_domaint::output_sys_size<<std::endl;
	item_id++;

	//break if you get an unsat constant constraint
	if (is_bottom(vs)){
		std::cout<<"final vector is bottom"<<std::endl;
		output<<"1,0\n";
		break;
	}

	}

	for(auto v:vs){

	if(v==vs.back()){
		//heavy data
		detailed_stats<<"number of result calls for "<<in_result->to_string()<<" is: "<<intermediate_result_call<<std::endl;
	}
			
	if(std::dynamic_pointer_cast<unary_inequality>(v)!=nullptr){
		std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(v);
		output<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";
	}
	else if(std::dynamic_pointer_cast<dyadic_inequality>(v)!=nullptr){
		std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(v);
		output<<d->x + ","+ d->y + "," + integer2string(d->a) + "," + integer2string(d->b) + ","+ integer2string(d->c)+ "\n";
	}

	}
	
	//std::cout<<"end of closure operation"<<std::endl;
	
	output.close();
	//changed
	//detailed_stats<<"total result calls: "<<tvpi_domaint::result_call<<std::endl;
	detailed_stats.close();
	plot_inputs_results.close();
	
}

std::vector<std::shared_ptr<inequality>> complete::closure(std::vector<std::shared_ptr<inequality>> cs){
	std::vector<std::shared_ptr<inequality>> vs;
	for(std::shared_ptr<inequality>c0:cs){

		//calculation of completion
		vs = complete::inc_complete(vs,c0);

		//break if you get an unsat constant constraint
		if (is_bottom(vs)){
			std::cout<<"final vector is bottom"<<std::endl;
			std::cout<<"1<0"<<std::endl;
			break;
		}

	}
	return vs;
}

std::vector<std::shared_ptr<inequality>> complete::insert(std::vector<std::shared_ptr<inequality>> cs){

    std::set<std::string> set;
    for (size_t i = 0; i < cs.size(); i++){

        auto vars_pet_ineq = cs.at(i)->vars();
        set.insert(vars_pet_ineq.begin(),vars_pet_ineq.end());

    }
    
    std::vector<std::string> vars(set.begin(),set.end());

	std::vector<std::shared_ptr<inequality>> compact;

	for (size_t i = 0; i < cs.size(); i++){

	    compact = insert(cs.at(i), compact, vars);
	    if (compact.empty()){ 
		break;
	    }
	    else{

        //std::cout<<"TBA"<<std::endl;
        //System.out.println(cs.get(i) + " -> " + compact);
		
	    }
	}
	return compact;
}

std::vector<std::shared_ptr<inequality>> complete::insert(std::shared_ptr<inequality> c, std::vector<std::shared_ptr<inequality>> cs, std::vector<std::string> vars){
	
    if (cs.size() == 0){
	    cs.push_back(c);
	    return cs;
	}
    else{
	
    cs.push_back(c);


	if(debugging){
	//std::cout<<"cs before comp:";
	print_ins(cs);
	}
	

    std::stable_sort(cs.begin(),cs.end(), planar_comparator(vars));

	if(debugging){
	//std::cout<<"cs before removal: ";
	print_ins(cs);
	}
	
    int index;
    auto it = std::find (cs.begin(), cs.end(), c);
    if(it!=cs.end()) index = it - cs.begin();
    else index = -1;

	//std::cout<<"index: ";
	//std::cout<<index<<std::endl;

	cs.erase(cs.begin()+index);
	//std::cout<<cs.size()<<std::endl;

	if(debugging){
	//std::cout<<"cs after rem: ";
	print_ins(cs);
	}
	


	//if (debugging) System.out.println("cs = " + cs); 

	int l = cs.size();
	int before = modulo(index - 1, l);
	int after = modulo(index, l);

	if(debugging){
	//std::cout<<"before:"<<before<<" after:"<<after<<std::endl;
    //std::cout<<cs[before]->to_string()<<" "<<cs[after]->to_string()<<std::endl;
	//std::cout<<"entailment checked on inequality: "<<c->to_string()<<std::endl;
	}

	if (c->planar_entailed(cs.at(before), cs.at(after))){
		if(debugging){
		//std::cout<<"c is entailed"<<std::endl;
		}
		return cs;
    }
    else{

		if(debugging){
			//std::cout<<"not entailed"<<std::endl;
		}
    	std::vector<std::shared_ptr<inequality>> ds;

		if (cs.size() == 1){
            
		    std::shared_ptr<inequality> c0 = cs.at(0);

	        if (c->planar_entailed(c0)){
			    ds.push_back(c0);
		    }
		    else if (c->planar_unsatisfiable(c0)){
				std::cout<<c->to_string()<<" is unsat"<<std::endl;
			    return make_bottom();
		    }
		}
		else
		{
		    //if (debugging) System.out.println("l, i = " + l + ", " + i);

		    bool entailed_before = true;
		    int m = 0;
            do {
			if(debugging){
			//std::cout<<"m do"<<std::endl;
			}
			m++;
            int i1 = modulo(index - m, l);
            int i2 = modulo(index - m - 1, l);
            std::shared_ptr<inequality> t1 = cs.at(i1);
			std::shared_ptr<inequality> t2 = cs.at(i2);
			if(debugging){
			//std::cout<<"c: "<<c->to_string()<<" t1: "<<t1->to_string()<<" t2: "<<t2->to_string()<<std::endl;
			}
			entailed_before = t1->planar_entailed(t2, c);
			//if (debugging) System.out.println(m + ": {" + i2 + ": " + t2 + ", " + c + "} |= " + i1 + ": " + t1 + " (" + entailed_before + ")");
		   	if(debugging){
			//std::cout<<"m: "<<m<<" l:"<<l<<" entailed_before: "<<entailed_before<<std::endl;
			}

		   
		    }
		    while (m < l && entailed_before);
 
		    bool entailed_after = true;
		    int n = -1;
		    do
		    {
			if(debugging){
			//std::cout<<"n do"<<std::endl;
			}
			n++;
            int i1 = modulo(index + n, l);
            int i2 = modulo(index + n + 1, l);
			std::shared_ptr<inequality> t1 = cs.at(i1);
			std::shared_ptr<inequality>  t2 = cs.at(i2);
			if(debugging){
			//std::cout<<"c: "<<c->to_string()<<" t1: "<<t1->to_string()<<" t2: "<<t2->to_string()<<std::endl;
			}
			entailed_after = t1->planar_entailed(c, t2);

			//if (debugging) System.out.println(n + ": {" + c + ", " + i2 + ": " + t2 + "} |= " + i1 + ": " + t1 + " [" + entailed_after + "]");
		     	if(debugging){
				//std::cout<<"n: "<<n<<" l:"<<l<<" entailed_after: "<<entailed_after<<std::endl;
				}
			} 
		    while (n < l && entailed_after);

		    //if (debugging) System.out.println("i, m, n = " + i + ", "+ m + ", " + n);

            int i1 = modulo(index - m, l);
            int i2 = modulo(index + n, l);
		    std::shared_ptr<inequality> t1 = cs.at(i1);
		    std::shared_ptr<inequality> t2 = cs.at(i2);
		    bool unsat = c->planar_unsatisfiable(t1, t2);
		    //if (debugging) System.out.println("unsat: " + c + ", " + t1 + ", " + t2 + " [" + unsat + "]");

		    if (unsat)
		    {
			return make_bottom();
		    }
		    else
		    {	
				if(debugging){
				//std::cout<<"m: "<<m<<" n: "<<n<<std::endl;
				}
			for (int k = 1 - m; k <= n - 1; k++)
			{
			    ds.push_back(cs.at(modulo(index+ k, l)));
			}
		
		    }
		}

		std::cout<<"ds to be removed after entailment"<<std::endl;
		print_cons(ds);

	    remove_all(cs,ds);
	    cs.push_back(c);
	    std::stable_sort(cs.begin(),cs.end(), planar_comparator(vars));
		return cs;
        }
    }
} 

std::vector<std::shared_ptr<inequality>> complete::result(std::vector<std::shared_ptr<inequality>> cs1, std::vector<std::shared_ptr<inequality>> cs2){
    
	std::vector<std::shared_ptr<inequality>> res;
	
	for (auto i2:cs2){
		std::vector<std::shared_ptr<inequality>> ineqs = result(cs1, i2);
		res.insert(res.end(),ineqs.begin(),ineqs.end());
	}
	return res;
}

std::vector<std::shared_ptr<inequality>> complete::result(std::vector<std::shared_ptr<inequality>> cs, std::shared_ptr<inequality> i2){
    
	std::vector<std::shared_ptr<inequality>> res;

	for (auto i1:cs){
		std::vector<std::shared_ptr<inequality>> ineqs = result(i1,i2);
		res.insert(res.end(),ineqs.begin(),ineqs.end());
	}

	return res;
} 

std::vector<std::shared_ptr<inequality>> complete::result(std::shared_ptr<inequality>  i1, std::shared_ptr<inequality>  i2){
	mp_integer a1, b1, c1, a2, b2, c2;
	std::string x1, y1, x2, y2;

	//register the result call
	//changed
	//tvpi_domaint::result_call +=1; 

	if (std::dynamic_pointer_cast<unary_inequality>(i1)!=nullptr){
		
		std::shared_ptr<unary_inequality> u1  = std::dynamic_pointer_cast<unary_inequality>(i1);
	 
	    a1 = u1->a;
	    b1 = 0;
	    c1 = u1->c;
	    x1 = u1->x;
	    y1 = "";
	}
	else
	{
		std::shared_ptr<dyadic_inequality> d1  = std::dynamic_pointer_cast<dyadic_inequality>(i1);
	    
	    a1 = d1->a;
	    b1 = d1->b;
	    c1 = d1->c;
	    x1 = d1->x;
	    y1 = d1->y;
	}

	if (std::dynamic_pointer_cast<unary_inequality>(i2)!=nullptr)
	{
		std::shared_ptr<unary_inequality> u2  = std::dynamic_pointer_cast<unary_inequality>(i2);
	   
	    a2 = u2->a;
	    b2 = 0;
	    c2 = u2->c;
	    x2 = u2->x;
	    y2 = "";
	}
	else
	{
		std::shared_ptr<dyadic_inequality> d2  = std::dynamic_pointer_cast<dyadic_inequality>(i2);
	    a2 = d2->a;
	    b2 = d2->b;
	    c2 = d2->c;
	    x2 = d2->x;
	    y2 = d2->y;
	}

    std::vector<std::shared_ptr<inequality>> result;
	if (sign(a1) * sign(a2) < 0 && x1==x2)
	{
	    mp_integer a1_abs = abs(a1);
	    mp_integer a2_abs = abs(a2);
	    mp_integer c = (a2_abs*c1)+(a1_abs*c2);
	    result.push_back(inequality_factory::make_inequality(y1, y2, a2_abs*b1, a1_abs*b2, c));
	}
	else if (sign(a1) * sign(b2) < 0 && x1==y2)
	{
	    mp_integer a1_abs = abs(a1);
	    mp_integer b2_abs = abs(b2);
	    mp_integer c = (b2_abs*c1)+(a1_abs*c2);
	    result.push_back(inequality_factory::make_inequality(y1, x2, b2_abs*b1, a1_abs*a2, c));
	}

	if (sign(b1) * sign(a2) < 0 && y1==x2)
	{
	    mp_integer b1_abs = abs(b1);
	    mp_integer a2_abs = abs(a2);
	    mp_integer c = (a2_abs*c1)+(b1_abs*c2);
	    result.push_back(inequality_factory::make_inequality(x1, y2, a2_abs*a1, b1_abs*b2, c));
	}
	else if (sign(b1) * sign(b2) < 0 && y1==y2)
	{
	    mp_integer b1_abs = abs(b1);
	    mp_integer b2_abs = abs(b2);
	    mp_integer c = (b2_abs*c1)+(b1_abs*c2);
	    result.push_back(inequality_factory::make_inequality(x1, x2, b2_abs*a1, b1_abs*a2, c));
	}

	//detailed_stats<<"result of: "<< i1->to_string()<<", "<<i2->to_string()<<std::endl;
	//std::cout<<i1->to_string()<<std::endl;
	
	//std::cout<<"ineq "<<i1->to_string()<<std::endl;

	if(in_result)
	{
			if(in_result==i1)
			{	
				intermediate_result_call+=1;
			}
			else
			{	
				//std::cout<<"number of result calls for "<<in_result->to_string()<<" is: "<<intermediate_result_call<<std::endl;
				detailed_stats<<"number of result calls for "<<in_result->to_string()<<" is: "<<intermediate_result_call<<std::endl;
				intermediate_result_call = 1;
				in_result = i1;
			}
	}
	else
	{
			//std::cout<<"empty"<<std::endl;
			in_result = i1;
			intermediate_result_call = 1;
			
	}
	std::cout<<"result of: "<<i1->to_string()<<" "<<i2->to_string()<<std::endl;
	std::cout<<"Low level res: ";
	print_cons(result);
	return result;

}

int complete::sign(mp_integer a){
        return compare_to(a,0);
}

std::vector<std::shared_ptr<inequality>> complete::project(std::vector<std::shared_ptr<inequality>> cs, std::vector<std::string> vars){
	
	//std::cout<<"project: "<<std::endl;
	std::set<std::string> set (vars.begin(),vars.end());
	//std::cout<<"set: ";
	for(auto it = set.begin();it !=set.end();it++){
		//std::cout<<' '<<*it;
	}
	//std::cout<<std::endl;

	std::vector<std::shared_ptr<inequality>> project;

	//std::cout<<"cs size in project" + std::to_string(cs.size())<<std::endl;

	for (auto i:cs)
	{
	  auto contents = i->vars();
	  //std::cout<<"vars per ineq:"; 
	  //print_vars(contents);
	  //std::cout<<std::endl;
	    if (std::includes(set.begin(),set.end(),contents.begin(),contents.end())) project.push_back(i);
	}
	return project;
}

std::vector<std::shared_ptr<inequality>> complete::remove_constant_inequalities(std::vector<std::shared_ptr<inequality>> cs){
        
	std::vector<std::shared_ptr<inequality>> fs;
	for (auto c:cs){
	   
	    if (std::dynamic_pointer_cast<constant_inequality>(c)!=nullptr){

		std::shared_ptr<constant_inequality> ci  = std::dynamic_pointer_cast<constant_inequality>(c);
		//std::cout<<"constant is: "<<ci->to_string()<<std::endl;
		if(ci->sat==false) return make_bottom();

		}
	    else fs.push_back(c);
	}
	return fs;
}

std::vector<std::shared_ptr<inequality>> complete::remove_redundant_unary_inequalities(std::vector<std::shared_ptr<inequality>> cs){
        std::vector<std::shared_ptr<inequality>> rcs;
        std::vector<std::shared_ptr<inequality>> us;

	std::cout<<"in redundant unary"<<std::endl;
	print_cons(rcs);


	for (auto c:cs)
	{
	    if (std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr) us.push_back(c);
	    else rcs.push_back(c);
	}


        std::vector<std::shared_ptr<inequality>> rus;
	for (size_t i = 0; i < us.size(); i++)
	{
	    bool entailed = false;
	    for (size_t j = 0; j < us.size(); j++)	
	    {
		if (i != j) entailed = entailed || us.at(i)->planar_entailed(us.at(j));
	    }	 
	    if (!entailed) rcs.push_back(us.at(i));
	}

	print_cons(rcs);
	return rcs;
}

std::vector<std::shared_ptr<inequality>> complete::inc_complete(std::vector<std::shared_ptr<inequality>> cs, std::shared_ptr<inequality> c0){
    
	std::vector<std::shared_ptr<inequality>> as;
	as.push_back(c0);

	//System.out.println("cs, c0 = " + cs + ", " + c0); 
	 	
	for (auto i:cs)
	{	
		
		std::vector<std::shared_ptr<inequality>> rs = remove_constant_inequalities(result(c0, i));

		if(!rs.empty()){
			//std::cout<<"rs arity: "<<rs[0]->arity()<<std::endl;
			if(is_bottom(rs)) return make_bottom();
		}

		std::vector<std::shared_ptr<inequality>> ts = remove_constant_inequalities(result(rs, cs));

	
		if(!ts.empty()){
			//std::cout<<"ts arity: "<<ts[0]->arity()<<std::endl;
			if(is_bottom(ts)) return make_bottom();
		}

		as.insert(as.end(),rs.begin(),rs.end());
		as.insert(as.end(),ts.begin(),ts.end());

	}

	if(debugging){
	////std::cout<<"as: ";
	}
	
	if(debugging){
	print_ins(as);
	}

	std::set<std::string> vars;
	for (auto i:as)
	{
	  auto vss = i->vars();
	  vars.insert(vss.begin(),vss.end());
	
	}

	if(debugging){
	//std::cout<<"vars:";
	}
	for(auto it = vars.begin();it !=vars.end();it++){
		if(debugging){
		std::cout<<' '<<*it;
		}
	}
	if(debugging){
	//std::cout<<std::endl;
	}
		
	//System.out.println("Y = {" + x + ", " + y + "}"); 
	// Avoid poking a project unnecessarily 

	if(debugging){
	//std::cout<<"cs before project:";
	print_ins(cs);
	}
	std::vector<std::string> change_vars(vars.begin(),vars.end());
	std::vector<std::shared_ptr<inequality>> change_cs = project(cs, change_vars);
	std::vector<std::shared_ptr<inequality>> residual_cs (cs);
	remove_all(residual_cs,change_cs);

	if(debugging){
	//std::cout<<"change_vars: ";
	print_vars(change_vars);

	//std::cout<<"change_cs: ";
	print_ins(change_cs);
	}

	for (std::string x:vars)
	{
	    for (std::string y:vars)
	    {
		if (x.compare(y) <= 0)
		{
		    std::vector<std::string> Y ={x,y};
		    std::vector<std::shared_ptr<inequality>> project_cs = project(change_cs, Y);
		    std::vector<std::shared_ptr<inequality>> project_as = project(as, Y);
		   //  System.out.println("project_cs = " + project_cs); 
		    // System.out.println("project_as = " + project_as); 

			std::cout<<"projected out"<<std::endl;
			print_cons(project_as);

		    for (auto i:project_as)
		    {
			project_cs = insert(i, project_cs, Y);
			if(debugging){
			//std::cout<<"project_cs: ";
			print_ins(project_cs);
			}
			if (is_bottom(project_cs)) return make_bottom();
		    }
			remove_all(residual_cs,project_cs);
	
		    residual_cs.insert(residual_cs.end(),project_cs.begin(),project_cs.end());
			
			if(debugging){
			//std::cout<<"residual cs:";
			print_ins(residual_cs);
			}

		    //System.out.println("Y, residual_cs = {" + x + ", " + y + "}, " + residual_cs); 
		}
	    }
	}


	detailed_stats<<"intermediate system: ";
	for (auto i:residual_cs){
		detailed_stats << i->to_string() <<"|";
	}
	detailed_stats<<std::endl;
	

	//fix is here
	residual_cs = remove_redundant_unary_inequalities(residual_cs);
	
	return residual_cs;

}

void remove_all(std::vector<std::shared_ptr<inequality>>&cs, std::vector<std::shared_ptr<inequality>>&ds){
    
	 for (auto it = cs.begin(); it != cs.end(); ) {
        if (std::find(ds.begin(), ds.end(), *it) != ds.end()) {
            it = cs.erase(it);
        } else {
            ++it;
			
        }
    }
}

extern std::vector<std::shared_ptr<inequality>> make_bottom(){
	//std::cout<<"make bottom"<<std::endl;
	return {std::make_shared<constant_inequality>(false)} ;
}

extern bool is_bottom(std::vector<std::shared_ptr<inequality>> vec){

	std::shared_ptr<inequality> in;
	if(!vec.empty()){
		in = vec[0];
	}
	
	if (std::dynamic_pointer_cast<constant_inequality>(in)!=nullptr){
		std::shared_ptr<constant_inequality> c  = std::dynamic_pointer_cast<constant_inequality>(in);
		return c->sat == false;
	}
	else{
		return false; 
	}


}