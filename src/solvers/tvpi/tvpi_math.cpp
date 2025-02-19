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
  tvpi_systemt a;
  tvpi_systemt b;

  tvpi_domaint::binding_map left;
  tvpi_domaint::binding_map right;

  a.add_inequality(1, "d1", 1, "d3", 4);
  a.add_inequality(1, "d1", 1, "d7", 8);
  a.add_inequality(1, "d9", 0, "", 3);
  a.add_inequality(1, "d3", 0, "", 2);

  left.insert(std::make_pair(str2symex("x"), 1));
  left.insert(std::make_pair(str2symex("y"), 3));
  left.insert(std::make_pair(str2symex("z"), 7));
  left.insert(std::make_pair(str2symex("l"), 9));

  b.add_inequality(1, "d1", 1, "d5", 5);
  b.add_inequality(1, "d1", 1, "d8", 8);
  b.add_inequality(1, "d8", 1, "d5", 8);
  b.add_inequality(1, "d3", 0, "", 1);

  right.insert(std::make_pair(str2symex("x"), 1));
  right.insert(std::make_pair(str2symex("y"), 5));
  right.insert(std::make_pair(str2symex("z"), 8));
  right.insert(std::make_pair(str2symex("l"), 4));

  std::vector<std::string> vk;
  vk.push_back("d4");
  auto fil = filter(b, vk);
  std::cout << "filter is" << std::endl;
  print_cons(fil);

  std::cout << "before align left" << std::endl;
  for(auto item : left)
  {
    std::cout << item.first.get_identifier() << " " << item.second << std::endl;
  }

  std::cout << "before align right " << std::endl;
  for(auto item : right)
  {
    std::cout << item.first.get_identifier() << " " << item.second << std::endl;
  }

  std::cout << "cons before relabel" << std::endl;
  print_cons(b.constraints);

  align_bindings(left, right, a, b);

  std::cout << "after align left " << std::endl;
  for(auto item : right)
  {
    std::cout << item.first.get_identifier() << " " << item.second << std::endl;
  }

  std::cout << "after align right " << std::endl;
  for(auto item : right)
  {
    std::cout << item.first.get_identifier() << " " << item.second << std::endl;
  }

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


tvpi_systemt a;
a.add_inequality(1,"d1",0,"d",6);
a.add_inequality(-1,"d1",0,"d",-6);
a.add_inequality(1,"d2",0,"d",-1);
a.add_inequality(-1,"d2",0,"d",1);
}