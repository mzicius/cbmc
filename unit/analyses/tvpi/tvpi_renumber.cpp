/*******************************************************************\
 Module: Unit tests for tvpi_domaint::renumber
 Author: Marius Zicius
\*******************************************************************/

#include <testing-utils/use_catch.h>
#include <map>
#include <string>
#include "../../../src/analyses/ai_domain.h"
#include "../../../src/analyses/tvpi/tvpi_domaint.h"
#include "../../../src/solvers/tvpi/tvpi_systemt.h"

SCENARIO(
  "renumber tvpi_domaint",
  "[core][analyses][tvpi][renumber]"){
 GIVEN("renumbering two bindings for TVPI domains"){
    WHEN("renumbering bindings of same size"){

        tvpi_domaint a;
        run();
        /*
        tvpi_systemt a;
        tvpi_systemt b;

        tvpi_domaint::binding_map left;
        tvpi_domaint::binding_map right;

        a.add_inequality(1,"d1",1,"d3",4);
        a.add_inequality(1,"d1",1,"d7",8);
        a.add_inequality(1,"d4",0,"",3);
   
        left.insert(std::make_pair(str2symex("x"),1));
        left.insert(std::make_pair(str2symex("y"),3));
        left.insert(std::make_pair(str2symex("z"),7));
        left.insert(std::make_pair(str2symex("l"),4));
    
        b.add_inequality(1,"d1",1,"d5",4);
        b.add_inequality(1,"d1",1,"d8",8);
        b.add_inequality(1,"d4",0,"",3);

        left.insert(std::make_pair(str2symex("x"),1));
        left.insert(std::make_pair(str2symex("y"),5));
        left.insert(std::make_pair(str2symex("z"),8));
        left.insert(std::make_pair(str2symex("l"),4));

        renumber(a,b,left,right);

        for (auto i: right){

            std::cout<<i.first<<" "<<i.second<<std::endl;

        }
        
        THEN("binding sizes are identical"){
 
        }
        */
    }
 }
}