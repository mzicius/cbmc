/* old comments
  //or a set of active dimension (live in tvpi_domain class not current)
  //var to mp_integer to tvpi sys
  //merge(x,y) look both tvpi sys and merge 
  //merge lookup approach
  //2D at a time 
  //Way one iterate the pairs 
  //Way two renumber the dimensions (match up) better
  //renumbering the TVPI system 
  //x + y < 7
  //2*x + 3*z < 5
  //change to 
  //a + b < 7
  //2*a + 3*c < 5
  //when you perform renumbering 
  //you must ensure that renumbring does not use reserved symbols
  //cant collapse dimensions
  //binding can be renumbred using entirely new keys 
**/

/* currently hidden functions
static void add(std::string ineq);
static bool project2D(std::string var);
static bool project(std::string var);
static size_t get_system_size();
static std::vector<std::shared_ptr<inequality>> get_system();
static void set_system(std::vector<std::shared_ptr<inequality>> new_sys);
static void clear_system();
static void print_system();
static void generate_randomly(std::string location,std::string dim, std::string constraints,std::string version);
static void generate_u(std::string location,std::string dim, std::string constraints,std::string version);
*/


//from cpp file 

/*
int main(int argc, char *argv[]){

    tvpi_domaint::total_duration = std::chrono::milliseconds(0);
    std::string option;
    
    while(getline(std::cin,option)){
        
        std::vector<std::string> criteria;
        std::stringstream ss(option);

        for(int i=0; getline(ss,option,' ');i++){
            criteria.push_back(option);
        }

        if(criteria.size()>=1){

        if(criteria[0]=="parse"){
            std::cout<<criteria[1]<<std::endl;
            std::vector<std::shared_ptr<inequality>> css;
            css = parser::parse_from_csv(criteria[1]);
            print_cons(css);
        }
        else if(criteria[0]=="add"){
            tvpi_domaint::add(criteria[1]);
        }
        else if(criteria[0]=="closure"){
            //std::cout<<criteria[1]<<std::endl;
            //std::cout<<criteria[2]<<std::endl;
            auto ts1 = std::chrono::high_resolution_clock::now();
            complete::closure(criteria[1],criteria[2]);
            auto ts2 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(ts2-ts1);
            //write stats
            std::ofstream stats;
            std::ofstream graph_data;
            std::string dim_label;
            std::string con_label;
            std::string version_label;
            
            std::string full_label = criteria[1];
            full_label = full_label.substr(full_label.find_last_of('/'));
            //<<"full label: "<<full_label<<std::endl;
            size_t last_fslash_pos = full_label.find_last_of('/');
            size_t first_dash_pos = full_label.find_first_of('-');
            size_t second_dash_pos = full_label.find('-',first_dash_pos+1);
            auto v_pos = full_label.find('v');
	        auto ex_pos = full_label.find(".in");
	        version_label = full_label.substr(v_pos,ex_pos-v_pos);


            dim_label = full_label.substr(last_fslash_pos+1,first_dash_pos-1);
            con_label = full_label.substr(first_dash_pos + 1, second_dash_pos - first_dash_pos - 1);
        
            std::string graph_name = "../../gnu/data-" + dim_label+ ".txt";
            //std::cout<<graph_name<<std::endl;
            graph_data.open(graph_name,std::ios_base::app);

            stats.open("../../test/general_stats.txt",std::ios_base::app);
            stats<< "input system: "<<criteria[1]<<std::endl;
            stats<< "output system: "<<criteria[2]<<std::endl;
            stats<< "input system size: "<<tvpi_domaint::input_sys_size<<std::endl;
            stats<< "output system size: "<<tvpi_domaint::output_sys_size<<std::endl;
            stats << "no. of results calls: "<<tvpi_domaint::result_call<<std::endl;
            //graph_data<<tvpi_domaint::input_sys_size<<","<<tvpi_domaint::output_sys_size<<","<<tvpi_domaint::result_call<<std::endl;

            graph_data<<con_label<<","<<duration.count()<<std::endl;
            stats << "duration of completion operation: "<<duration.count()<<"ms"<<std::endl;
            tvpi_domaint::total_duration+= duration;
            stats << "total duration so far:"<<tvpi_domaint::total_duration.count()<<"ms"<<std::endl;

            //new plots
            //time vs input system
            std::ofstream plot_time_input;
            plot_time_input.open("../../plots/"+dim_label+"/"+dim_label+"D-TI.txt",std::ios_base::app);
            plot_time_input<<duration.count()<<",";
            plot_time_input<<tvpi_domaint::input_sys_size<<std::endl;

            //std::cout<<"../../plots/"+dim_label+"/"+dim_label+"D-TI.txt"<<std::endl;

            //time vs result calls 
            std::ofstream plot_time_result;
            plot_time_result.open("../../plots/"+dim_label+"/"+dim_label+"D-TR.txt",std::ios_base::app);
            plot_time_result<<duration.count()<<",";
            plot_time_result<<tvpi_domaint::result_call<<std::endl;

            //std::cout<<"../../plots/"+dim_label+"/"+dim_label+"D-TR.txt"<<std::endl;

        }
        else if(criteria[0]=="hull"){
            hull::join(criteria[1],criteria[2],criteria[3]);
        }
        else if(criteria[0]=="project2D"){
            tvpi_domaint::project2D(criteria[1]);
        }
        else if(criteria[0]=="project"){
            tvpi_domaint::project(criteria[1]);
        }
        else if(criteria[0]=="random"){
            tvpi_domaint::generate_randomly(criteria[1],criteria[2],criteria[3],criteria[4]);
        }
        else if(criteria[0]=="random_un"){
            tvpi_domaint::generate_u(criteria[1],criteria[2],criteria[3],criteria[4]);
        }
        else if(criteria[0]=="print_tikz"){
            std::vector<std::shared_ptr<inequality>> cs = parser::parse_from_csv(criteria[1]);
            tikz_printer::print_tikz(cs,criteria[2]);
        }
        else if(criteria[0]=="print_tikz_hull"){
            std::vector<std::shared_ptr<inequality>> cs1 = parser::parse_from_csv(criteria[1]);
            std::vector<std::shared_ptr<inequality>> cs2 = parser::parse_from_csv(criteria[2]);
            std::vector<std::shared_ptr<inequality>> out = parser::parse_from_csv(criteria[3]);
            tikz_printer::print_tikz_hull(cs1,cs2,out,criteria[4]);

        }
        else if(criteria[0]=="print"){
            smt_printer::print_system(criteria[1],criteria[2]);
        }
        else if(criteria[0]=="print_clo"){
            smt_printer::print_smt_closure(criteria[1],criteria[2],criteria[3]);
        }
        else if(criteria[0]=="print_tikz_sys"){
            std::cout<<"test"<<std::endl;
            std::vector<std::shared_ptr<inequality>> cs = parser::parse_from_csv(criteria[1]);
            std::cout<<cs.size()<<std::endl;
            tikz_printer::print_tikz_system(cs,criteria[2],criteria[3],std::stoi(criteria[4]));
        }
        else if(criteria[0]=="print_hull"){
            std::cout<<"here"<<std::endl;
            smt_printer::print_hull(criteria[1],criteria[2],criteria[3],criteria[4]);
        }
        }


       //std::cout<<"The current system:"<<std::endl;
       //tvpi_domaint::print_system();
       //std::cout<<"Size:"<<std::endl;
       //std::cout<<tvpi_domaint::get_system_size()<<std::endl;

    }
    
    


}
*/  

/*
/// @brief add inequality to system and compute closure
/// @param c 
void tvpi_domaint::add(std::string ineq){
    if(ineq[0]!='#'){
        //std::cout<<ineq[0]<<std::endl;
        tvpi_domaint::sys.push_back(parser::parse_inequality(ineq));
        tvpi_domaint::set_system(complete::closure(tvpi_domaint::sys));
    }
}
*/

/*
/// @brief return size of the tvpi_domaint system stored in memory
/// @return 
size_t tvpi_domaint::get_system_size(){
	return tvpi_domaint::sys.size();
}

/// @brief update the tvpi_domaint system stored in memory
/// @param new_sys 
void tvpi_domaint::set_system(std::vector<std::shared_ptr<inequality>> new_sys){
	tvpi_domaint::sys = new_sys;
}

/// @brief clear the tvpi_domaint system stored in memory
void tvpi_domaint::clear_system(){
	tvpi_domaint::sys.clear();
}

/// @brief print the tvpi_domaint system stored in memory
void tvpi_domaint::print_system(){
	for(size_t t=0; t<tvpi_domaint::sys.size();t++){
		//std::cout<<tvpi_domaint::sys[t]->to_string()<<std::endl;
	}
}

/// @brief 
/// @return the tvpi_domaint system stored in memory 
std::vector<std::shared_ptr<inequality>> tvpi_domaint::get_system(){
    return tvpi_domaint::sys;
}

/// @brief  projection to 2D using the given variables
bool tvpi_domaint::project2D(std::string var){

    std::vector<std::string> vars;
    std::stringstream ss(var);
    while(ss.good()){
        std::string label;
        getline(ss,label,',');
        vars.push_back(label);
    }

    std::vector<std::shared_ptr<inequality>> projection2D = complete::project(tvpi_domaint::get_system(),vars);
    if(!projection2D.empty()){

        tvpi_domaint::set_system(projection2D);
        return true;
    }
   
    tvpi_domaint::print_system();

    return false;
}

/// @brief projection that removes all system using the parameter variable
bool tvpi_domaint::project(std::string var){

    bool result = false;
    std::vector<std::shared_ptr<inequality>> project;

    for(std::shared_ptr<inequality> c:tvpi_domaint::sys){
        std::vector<std::string> contents = c->vars();
        auto loc = find(contents.begin(),contents.end(),var);
        if(loc==contents.end()){
            project.push_back(c);
            result = true;
        }
    }

    tvpi_domaint::set_system(project);

    return result;
}

void tvpi_domaint::generate_randomly(std::string location,std::string dim, std::string constraints,std::string version){

std::string rand_test_dir = "../../test/random/";
std::vector<std::shared_ptr<inequality>> cs;

std::string vars[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
			"k", "l", "m", "n", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z" };

//int logahedra[] = {-2, -1, 0, 1, 2};

auto cons = std::stoi(constraints);
auto dims = std::stoi(dim);


for (int i = 0; i < cons; i++){
	
        std::shared_ptr<inequality> in;
        do
        {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(-16, 15);
                std::uniform_int_distribution<> _dis(0, 32);
                std::uniform_int_distribution<> dis_(0, dims-1);
                int a = dis(gen);
                int b = dis(gen);
                int c = _dis(gen);

                //std::cout<<"a"<<a<<std::endl;
               
                in = inequality_factory::make_inequality(vars[dis_(gen)], vars[dis_(gen)], a, b, c);
        
        }
        while (std::dynamic_pointer_cast<constant_inequality>(in)!=nullptr);
        
cs.push_back(in);
}


std::ofstream out;
out.open(location + dim + "-" + constraints + "-v" + version + ".in");

for(auto c:cs){

if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
        std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);
        out<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";
}
else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
        std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
        out<<d->x + ","+ d->y + "," + integer2string(d->a) + "," + integer2string(d->b) + ","+ integer2string(d->c)+ "\n";
}

}

out.close();

}

void tvpi_domaint::generate_u(std::string location,std::string dim, std::string constraints,std::string version){

std::string rand_test_dir = "../../test/random/";
std::vector<std::shared_ptr<inequality>> cs;

std::string vars[] = { "a", "b", "c", "d", "e", "f", "g", "h", "i", "j",
			"k", "l", "m", "n", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z" };

//int logahedra[] = {-2, -1, 0, 1, 2};

auto cons = std::stoi(constraints);
auto dims = std::stoi(dim);


for (int i = 0; i < cons; i++){
	
        std::shared_ptr<inequality> in;
        do
        {
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(-16, 15);
                std::uniform_int_distribution<> _dis(-32, 32);
                std::uniform_int_distribution<> dis_(0, dims-1);
                int a = dis(gen);
                int b = dis(gen);
                int c = _dis(gen);

                //std::cout<<"a"<<a<<std::endl;
               
                in = inequality_factory::make_inequality(vars[dis_(gen)], vars[dis_(gen)], a, b, c);
        
        }
        while (std::dynamic_pointer_cast<constant_inequality>(in)!=nullptr);
        
cs.push_back(in);
}


std::ofstream out;
out.open(location + dim + "-" + constraints + "-v" + version + ".in");

for(auto c:cs){

if(std::dynamic_pointer_cast<unary_inequality>(c)!=nullptr){
        std::shared_ptr<unary_inequality> u  = std::dynamic_pointer_cast<unary_inequality>(c);
        out<<u->x+","+integer2string(u->a)+","+integer2string(u->c)+"\n";
}
else if(std::dynamic_pointer_cast<dyadic_inequality>(c)!=nullptr){
        std::shared_ptr<dyadic_inequality> d = std::dynamic_pointer_cast<dyadic_inequality>(c);
        out<<d->x + ","+ d->y + "," + integer2string(d->a) + "," + integer2string(d->b) + ","+ integer2string(d->c)+ "\n";
}

}

out.close();

}

*/