#include "join.h"
#include <iostream>


std::ofstream join::full_hull_trace;
int join::hull_ID = 0;
std::set<std::string> dimensions;

void join::rm_excess(
  std::shared_ptr<inequality> ref,
  std::vector<std::shared_ptr<inequality>> &ls)
{

  std::cout<<"ref: "<<ref->to_string()<<std::endl;
  std::vector<std::shared_ptr<inequality>> rev_ls(ls.rbegin(),ls.rend());
  auto pos = std::find_if_not(rev_ls.begin(),rev_ls.end(),[ref](std::shared_ptr<inequality> i){return cmp_angle(i,ref)!=inequality::GT;});

  std::vector<std::shared_ptr<inequality>> beginning(rev_ls.begin(), pos);
  std::vector<std::shared_ptr<inequality>> end(pos, rev_ls.end());

  std::reverse(beginning.begin(), beginning.end());
  std::reverse(end.begin(), end.end());
  
  beginning.insert(beginning.end(),end.begin(),end.end());

  ls = beginning;

}

std::vector<std::shared_ptr<inequality>> join::calc_hull(
  std::vector<std::shared_ptr<inequality>> xs,
  std::vector<std::shared_ptr<inequality>> ys)
{
  std::cout << "hull start function" << std::endl;

  /*
std::cout<<std::endl;
std::cout<<"xs: "<<std::endl;
print_cons(xs);
std::cout<<"ys: "<<std::endl;
print_cons(ys);
std::cout<<std::endl;
*/

  std::vector<std::shared_ptr<inequality>> res;

  //return whole plane if both inputs are empty
  if(xs.empty() || ys.empty())
  {
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

  switch(cmp_angle(x, y))
  {
  case inequality::EQ:

    if(calc_shift(x, y).get_numerator() > 0 && calc_shift(x, y).get_denominator() >0)
    { 
      std::cout<<"Entry 1"<<std::endl;
      advance_outer(res, xn, xs, yn, ys);
    }
    else
    {
      std::cout<<"Entry 2"<<std::endl;
      advance_outer(res, yn, ys, xn, xs);
    }
    rm_excess(x,res);
    break;
  case inequality::LT:
    dist_LT = calc_dist(yn, x, y);
    if(dist_LT.has_value()){
      std::cout<<dist_LT.value();
    }
    if(dist_LT.has_value() && is_negative(dist_LT.value()))
    { 
    
      std::cout<<"Entry 3"<<std::endl;
      advance_outer(res, yn, ys, xn, xs);
    }
    else
    {
      std::cout<<"Entry 4"<<std::endl;
      advance_outer(res, xn, xs, yn, ys);
    }
    rm_excess(x,res);
    break;
  case inequality::GT:
    dist_GT = calc_dist(xn, y, x);
    if(dist_GT.has_value() && is_negative(dist_GT.value()))
    {
      std::cout<<"Entry 5"<<std::endl;
      advance_outer(res, xn, xs, yn, ys);
    }
    else
    { if(dist_GT.has_value()){
      std::cout<<dist_GT.value()<<std::endl;
    }
      std::cout<<"Entry 6"<<std::endl;
      advance_outer(res, yn, ys, xn, xs);
    }
    rm_excess(y,res);
    break;
  }

  res = rm_syn_red(res);

  /* syntax redundancies 
std::cout<<"vector before clean up"<<std::endl;
print_cons(res);
*/

  res = rm_local_red(res);

  /* local redundancies 
std::cout<<"vector after local clean up"<<std::endl;
print_cons(res);
*/

  std::cout << "final res" << std::endl;
  print_cons(res);



  return res;
}

void join::merge(
  std::string in_dir_xs,
  std::string in_dir_ys,
  std::string out_dir)
{
  full_hull_trace.open("../../trace/trace.txt");

  std::cout << in_dir_xs << std::endl;
  std::cout << in_dir_ys << std::endl;

  std::vector<std::shared_ptr<inequality>> xs =
    parser::parse_from_csv(in_dir_xs);
  std::vector<std::shared_ptr<inequality>> ys =
    parser::parse_from_csv(in_dir_ys);

  std::vector<std::shared_ptr<inequality>> rs;

  rs = join::calc_hull(xs, ys);

  std::ofstream output;

  output.open(out_dir);

  for(auto v : rs)
  {
    if(std::dynamic_pointer_cast<unary_inequality>(v) != nullptr)
    {
      std::shared_ptr<unary_inequality> u =
        std::dynamic_pointer_cast<unary_inequality>(v);
      output << u->x + "," + integer2string(u->a) + "," + integer2string(u->c) +
                  "\n";
    }
    else if(std::dynamic_pointer_cast<dyadic_inequality>(v) != nullptr)
    {
      std::shared_ptr<dyadic_inequality> d =
        std::dynamic_pointer_cast<dyadic_inequality>(v);
      output << d->x + "," + d->y + "," + integer2string(d->a) + "," +
                  integer2string(d->b) + "," + integer2string(d->c) + "\n";
    }
  }

  full_hull_trace.close();
  output.close();
}

void extract_dimensions(const std::vector<std::shared_ptr<inequality>> &sys){

std::vector<std::string> vars;
std::set<std::string> dims;

for(const std::shared_ptr<inequality> &i: sys){

if(dimensions.size()>=2){
  break;
}

vars = i->vars();
dims = std::set<std::string>(vars.begin(),vars.end());

dimensions.insert(dims.begin(),dims.end());

}

}

void sweep_dimensions(){
  dimensions = {};
}