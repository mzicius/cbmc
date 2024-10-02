/*******************************************************************\

 Module: Unit tests for tvpi_domaint::merge

 Author: Martin Brain
 Co-Author: Marius Zicius
\*******************************************************************/

#include <testing-utils/use_catch.h>
#include <../src/tvpi/tvpi_domaint.h>
#include <../src/analyses/ai_domain.h>

SCENARIO(
  "merge tvpi_domaint",
  "[core][analyses][tvpi][merge]")
{ 
  GIVEN("merging two TVPI domains")
  {
    WHEN("merging bottom with bottom")
    {
      tvpi_domaint a;
      tvpi_domaint b;
      a.make_bottom();
      b.make_bottom();
    
      //placeholder pointers
      //are not used at the moment
      ai_domain_baset::trace_ptrt from;
      ai_domain_baset::trace_ptrt to;

      bool modified = a.merge(b,from,to);
      
      THEN("result is bottom and unmodified")
      {
        REQUIRE(a.is_bottom());
        REQUIRE(modified == false);
      }
    }

    WHEN("merging bottom with top")
    {
      tvpi_domaint a;
      tvpi_domaint b;
      a.make_bottom();
      b.make_top();

      //placeholder pointers
      //are not used at the moment
      ai_domain_baset::trace_ptrt from;
      ai_domain_baset::trace_ptrt to;


      bool modified = a.merge(b,from,to);
      
      THEN("result is top and modified")
      {
        REQUIRE(a.is_top());
        REQUIRE(modified == true);
      }
    }
    
    WHEN("merging top with bottom")
    {
      tvpi_domaint a;
      tvpi_domaint b;
      a.make_top();
      b.make_bottom();

      //placeholder pointers
      //are not used at the moment
      ai_domain_baset::trace_ptrt from;
      ai_domain_baset::trace_ptrt to;


      bool modified = a.merge(b,from,to);
      
      THEN("result is top and unmodified")
      {
        REQUIRE(a.is_top());
        REQUIRE(modified == false);
      }
    }
    
    WHEN("merging top with top")
    {
      tvpi_domaint a;
      tvpi_domaint b;
      a.make_top();
      b.make_bottom();

      //placeholder pointers
      //are not used at the moment
      ai_domain_baset::trace_ptrt from;
      ai_domain_baset::trace_ptrt to;


      bool modified = a.merge(b,from,to);
      
      THEN("result is top and unmodified")
      {
        REQUIRE(a.is_top());
        REQUIRE(modified == false);
      }
    }
  }
}