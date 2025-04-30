#include "inequality_factory.h"
#include "join.h"
#include "tvpi_systemt.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

#include "../../analyses/tvpi/tvpi_domaint.h"

int main(int argc, char *argv[])
{
  

  /*
  std::cout << "cons after relabel" << std::endl;

  print_cons(b.constraints);

  //building relations

  std::vector<std::shared_ptr<inequality>> convex_union;
  std::vector<std::shared_ptr<inequality>> int_union;

  std::set<std::string> existing_relations = find_relations(a,b);

  std::cout<<"the relations currently are:"<<std::endl;
  for(const std::string &s:existing_relations){
    std::cout<<s<<std::endl;
  }

  //check of arity in sets
  for(const auto &ineq: a.constraints){
    std::cout<<ineq->to_string()<<" has arity of:"<<ineq->arity()<<std::endl;
  }

  a.constraints = add_var(a);
  b.constraints = add_var(b);

  std::cout<<"arity check after normalizing"<<std::endl;
  for(const auto &ineq: a.constraints){
    std::cout<<ineq->to_string()<<" has arity of: "<<ineq->arity()<<std::endl;
  }

  std::cout << "relations" << std::endl;
  for(const auto &rel : existing_relations)
  {
    std::vector<std::shared_ptr<inequality>> filter_left;
    std::vector<std::shared_ptr<inequality>> filter_right;
    std::string label;
    std::vector<std::string> target_vars;
    auto dash = rel.find("-");
    std::string first_var = rel.substr(0, dash);
    std::string second_var = rel.substr(dash + 1);

    std::cout << first_var << " part2: " << second_var << std::endl;
    if(first_var != second_var)
    {
      target_vars = {first_var, second_var};
    }
    else
    {
      target_vars = {first_var, ""};
    }
    
    filter_left = filter(a, target_vars);
    filter_right = filter(b, target_vars);
    std::cout << "left filter dy" << std::endl;
    print_cons(filter_left);
    std::cout << "right filter dy" << std::endl;
    print_cons(filter_right);

    int_union = join::calc_hull(filter_left, filter_right);
    convex_union.insert(convex_union.end(), int_union.begin(), int_union.end());
    std::cout << "inter: " << std::endl;

  }



  std::cout << "convex: " << std::endl;
  print_cons(convex_union);



   std::cout<<"final check"<<std::endl;
  std::vector<std::shared_ptr<inequality>> left;
   std::vector<std::shared_ptr<inequality>> right;
   std::vector<std::shared_ptr<inequality>> cross;

  std::shared_ptr<inequality> a = inequality_factory::make_inequality("x",5,7);
  std::shared_ptr<inequality> b = inequality_factory::make_inequality("y",1,7);
  std::shared_ptr<inequality> c = inequality_factory::make_inequality("x",-5,7);
  std::shared_ptr<inequality> d = inequality_factory::make_inequality("y",-1,7);

  std::shared_ptr<inequality> x = inequality_factory::make_inequality("x",1,11);
  std::shared_ptr<inequality> y = inequality_factory::make_inequality("y",2,3);
  std::shared_ptr<inequality> z = inequality_factory::make_inequality("x",-1,11);
  std::shared_ptr<inequality> q = inequality_factory::make_inequality("y",-2,3);

  sweep_dimensions();

  left.push_back(a);
    left.push_back(b);
      left.push_back(c);
        left.push_back(d);

          right.push_back(x);
            right.push_back(y);
              right.push_back(z);
                right.push_back(q);


extract_dimensions(left);
extract_dimensions(right);

for(const std::string &v : dimensions){
  std::cout<<v<<std::endl;
}

cross = join::calc_hull(left,right);
return 0;
  */

  /*
  tvpi_systemt a;
  a.add_inequality(1, "d1", 0, "d", 6);
  a.add_inequality(1, "d2", 0, "d", -1);
  a.add_inequality(-1, "d4", 0, "d", 1);
  a.add_inequality(-1,"d4",1,"d5",3);
  
  //std::shared_ptr<inequality> i1 = inequality_factory::make_inequality()

  tvpi_systemt b;
  b.add_inequality(1, "d1", 0, "d", 6);
  b.add_inequality(-1, "d7", 0, "d", 1);
  b.add_inequality(-1, "d4", 0, "d", 1);

  std::cout << std::endl;
  std::cout << "system a once created" << std::endl;
  print_cons(a.constraints);

  std::cout << "system b once created" << std::endl;
  print_cons(b.constraints);

 

  
  std::vector<std::shared_ptr<inequality>> intersection;

  for(auto con_a : a.constraints){
    for(auto con_b : b.constraints){
      std::cout<<"con_a: "<<con_a->to_string()<<" con_b: "<<con_b->to_string()<<std::endl;
      if(con_a->to_string() == con_b->to_string()){
        std::cout<<"same"<<std::endl;
        intersection.push_back(con_a);
      }
    }
  }
  

  tvpi_systemt a;
  a.add_inequality(1, "d1", 0, "d", 6);
  a.add_inequality(1, "d2", 0, "d", -1);
  a.add_inequality(-1, "d4", 0, "d", 1);
  a.add_inequality(-1,"d4",1,"d5",3);
  a.add_inequality(-1,"d1",1,"d5",3);
  
  //std::shared_ptr<inequality> i1 = inequality_factory::make_inequality()

  tvpi_systemt b;
  b.add_inequality(1, "d1", 0, "d", 6);
  b.add_inequality(-1, "d1", 0, "d", -6);
  b.add_inequality(-1, "d4", 0, "d", 1);


  std::shared_ptr<inequality> ia = inequality_factory::make_inequality("x","y",1,2,3);
  std::shared_ptr<inequality> ib = inequality_factory::make_inequality("y","x",2,3,4);

  std::vector<std::string> vars_a = ia->vars();
  std::vector<std::string> vars_b = ib->vars();


  if(vars_a == vars_b){
    std::cout<<"equal vectors"<<std::endl;
  }
  
  std::vector<std::shared_ptr<inequality>> left;
  std::vector<std::shared_ptr<inequality>> right;

  left = a.filter({"d1","d5"});
  right = a.filter({"d4","d5"});

  print_cons(left);
  std::cout<<std::endl;
  print_cons(right);

  std::cout<<a.get_ub(1).value()<<std::endl;
  std::cout<<a.get_lb(1).value()<<std::endl;

  //print_cons(intersection);
 */


  //rational factory start


  std::shared_ptr<inequality> ri1 = inequality_factory::make_inequality("x","y",rationalt(0),rationalt(7)/rationalt(3),rationalt(9)/rationalt(3));
  print_ineq(ri1);
  std::shared_ptr<inequality> ri2 = inequality_factory::make_inequality("x","y",rationalt(7)/rationalt(3),rationalt(0),rationalt(9)/rationalt(4));
  print_ineq(ri2);
  std::shared_ptr<inequality> ri3 = inequality_factory::make_inequality("x","x",rationalt(9)/rationalt(8),rationalt(7)/rationalt(3),rationalt(9)/rationalt(3));
  print_ineq(ri3);
  std::shared_ptr<inequality> ri4 = inequality_factory::make_inequality("x","y",rationalt(5)/rationalt(2),rationalt(7)/rationalt(3),rationalt(9)/rationalt(3));
  print_ineq(ri4);
  std::shared_ptr<inequality> ri5 = inequality_factory::make_inequality("y","x",rationalt(5)/rationalt(2),rationalt(7)/rationalt(3),rationalt(9)/rationalt(3));
  print_ineq(ri5);
  std::shared_ptr<inequality> ri6 = inequality_factory::make_inequality("x",rationalt(0),rationalt(7)/rationalt(3));
  print_ineq(ri6);
  std::shared_ptr<inequality> ri7 = inequality_factory::make_inequality("x",rationalt(1)/rationalt(3),rationalt(7)/rationalt(3));
  print_ineq(ri7);
  std::shared_ptr<inequality> ri8 = inequality_factory::make_inequality("x",rationalt(2)/rationalt(9),rationalt(6)/rationalt(7));
  print_ineq(ri8);


  //rational factory end

  /*
  //bounds start

  tvpi_systemt a;
  tvpi_systemt b;

  //a.add_inequality(2,"d1",0,"d2",3);
  //a.add_inequality(-2,"d1",0,"d2",-3);
  //a.add_inequality(rationalt(1),"d1",rationalt(0),"d2",rationalt(3)/rationalt(2));
  //a.add_inequality(rationalt(-1),"d1",rationalt(0),"d2",rationalt(1)/rationalt(2));

  //a.add_inequality(rationalt(1),"d1",rationalt(2),"d2",rationalt(3)/rationalt(2));
  //a.add_inequality(rationalt(-1),"d1",rationalt(-2),"d2",rationalt(1)/rationalt(2));

  //b.add_inequality(2,"d1",0,"d2",-3);
  //b.add_inequality(-2,"d1",0,"d2",3);

 


  if(a.get_ub(1).has_value()){
    std::cout<<"ub in a on dim 1 in sys a is: "<<a.get_ub(1).value()<<std::endl;
  }

  if(a.get_lb(1).has_value()){
    std::cout<<"lb in a on dim 1 in sys a is: "<<a.get_lb(1).value()<<std::endl;
  }

  if(b.get_ub(1).has_value()){
    std::cout<<"ub in a on dim 1 in sys b is: "<<b.get_ub(1).value()<<std::endl;
  }

  if(b.get_lb(1).has_value()){
    std::cout<<"lb in a on dim 1 in sys b is: "<<b.get_lb(1).value()<<std::endl;
  }


  //bounds end
  */

  //align start

  tvpi_systemt a;
  tvpi_systemt b;

  tvpi_bindingt left;
  tvpi_bindingt right;

  a.add_inequality(1, "d1", 1, "d3", 4);
  a.add_inequality(1, "d1", 1, "d7", 8);
  a.add_inequality(1, "d9", 0, "", 3);
  a.add_inequality(1, "d3", 0, "", 2);

  left.set_binding(str2symex("x"), 1);
  left.set_binding(str2symex("y"), 3);
  left.set_binding(str2symex("z"), 7);
  left.set_binding(str2symex("l"), 9);
  left.set_binding(str2symex("r"), 11);

  b.add_inequality(1, "d1", 1, "d5", 5);
  b.add_inequality(1, "d1", 1, "d8", 8);
  b.add_inequality(1, "d8", 1, "d5", 8);


  right.set_binding(str2symex("x"), 1);
  right.set_binding(str2symex("y"), 5);
  right.set_binding(str2symex("z"), 8);
  right.set_binding(str2symex("l"), 4);

  std::cout<<std::endl;
  std::cout << "before align left" << std::endl;
  left.print_binding();

  std::cout << "cons a before relabel" << std::endl;
  print_cons(a.constraints);

  std::cout << "before align right " << std::endl;
  right.print_binding();

  std::cout << "cons b before relabel" << std::endl;
  print_cons(b.constraints);

  align_bindings(left.binding,right.binding, a, b);

  std::cout << "after align left " << std::endl;
  left.print_binding();

  std::cout << "cons a after relabel" << std::endl;
  print_cons(a.constraints);

  std::cout << "after align right " << std::endl;
  right.print_binding();

  std::cout << "cons b after relabel" << std::endl;
  print_cons(b.constraints);


  std::set<std::string> existing_relations = find_relations(a, b);

  std::vector<std::shared_ptr<inequality>> interm_union;
  std::vector<std::shared_ptr<inequality>> convex_union;

  for(const auto &rel : existing_relations)
  {
    sweep_dimensions();
    std::vector<std::shared_ptr<inequality>> filter_left;
    std::vector<std::shared_ptr<inequality>> filter_right;
    std::string label;
    std::vector<std::string> target_vars;
    auto dash = rel.find("-");
    std::string first_var = rel.substr(0, dash);
    std::string second_var = rel.substr(dash + 1);

    //std::cout << first_var << " part2: " << second_var << std::endl;
    if(first_var != second_var)
    {
      //std::cout << "first var: " << first_var << " second var: " << second_var
      //          << std::endl;
      target_vars = {first_var, second_var};
    }
    else
    {
      target_vars = {first_var};
    }

    filter_left = a.filter(target_vars);
    filter_right = b.filter(target_vars);

    std::cout << "the target vars for filter are: :" << std::endl;
    for(const auto &var : target_vars)
    {
      std::cout << var << std::endl;
    }

    std::cout << "left filter" << std::endl;
    print_cons(filter_left);
    std::cout << "right filter" << std::endl;
    print_cons(filter_right);
    extract_dimensions(filter_left);
    extract_dimensions(filter_right);
    interm_union = join::calc_hull(filter_left, filter_right);
    std::cout << "inter convex hull is: " << std::endl;
    print_cons(interm_union);
    convex_union.insert(
      convex_union.end(), interm_union.begin(), interm_union.end());
  }

  std::cout<<"final convex union is: "<<std::endl;
  print_cons(convex_union);


  tvpi_systemt c;
  tvpi_systemt d;

  c.add_inequality(1,"d17",0,"",11);
  d.add_inequality(-1,"d17",0,"",-4);

  auto in = join::calc_hull(c.constraints,d.constraints);
  std::cout<<"in is:"<<std::endl;
  print_cons(in);
  
}
