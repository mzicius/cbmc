/*******************************************************************\

Module: Goto-Analyzer Command Line Option Processing

Author: Martin Brain, martin.brain@cs.ox.ac.uk

\*******************************************************************/

#include "build_analyzer.h"
#include <util/options.h>
#include <goto-programs/goto_model.h>
#include <analyses/ai.h>
#include <analyses/call_stack_history.h>
#include <analyses/constant_propagator.h>
#include <analyses/dependence_graph.h>
#include <analyses/interval_domain.h>
#include <analyses/example_domain.h>
#include <analyses/local_control_flow_history.h>
#include <analyses/variable-sensitivity/three_way_merge_abstract_interpreter.h>
#include <analyses/variable-sensitivity/variable_sensitivity_configuration.h>
#include <analyses/variable-sensitivity/variable_sensitivity_dependence_graph.h>
#include <analyses/variable-sensitivity/variable_sensitivity_domain.h>
#include <analyses/variable-sensitivity/variable_sensitivity_object_factory.h>
#include <analyses/tvpi/tvpi_domaint.h>

/// Ideally this should be a pure function of options.
/// However at the moment some domains require the goto_model or parts of it
std::unique_ptr<ai_baset> build_analyzer(
  const optionst &options,
  const goto_modelt &goto_model,
  const namespacet &ns,
  message_handlert &mh)
{
  auto vsd_config = vsd_configt::from_options(options);
  auto vs_object_factory =
    variable_sensitivity_object_factoryt::configured_with(vsd_config);

  // These support all of the option categories
  if(
    options.get_bool_option("recursive-interprocedural") ||
    options.get_bool_option("three-way-merge"))
  {
    // Build the history factory
    std::unique_ptr<ai_history_factory_baset> hf = nullptr;
    if(options.get_bool_option("ahistorical"))
    {
      hf = std::make_unique<
        ai_history_factory_default_constructort<ahistoricalt>>();
    }
    else if(options.get_bool_option("call-stack"))
    {
      hf = std::make_unique<call_stack_history_factoryt>(
        options.get_unsigned_int_option("call-stack-recursion-limit"));
    }
    else if(options.get_bool_option("local-control-flow-history"))
    {
      hf = std::make_unique<local_control_flow_history_factoryt>(
        options.get_bool_option("local-control-flow-history-forward"),
        options.get_bool_option("local-control-flow-history-backward"),
        options.get_unsigned_int_option("local-control-flow-history-limit"));
    }

    // Build the domain factory
    std::unique_ptr<ai_domain_factory_baset> df = nullptr;
    if(options.get_bool_option("constants"))
    {
      df = std::make_unique<
        ai_domain_factory_default_constructort<constant_propagator_domaint>>();
    }
    else if(options.get_bool_option("intervals"))
    {
      df = std::make_unique<
        ai_domain_factory_default_constructort<interval_domaint>>();
    }
    else if(options.get_bool_option("vsd"))
    {
      df = std::make_unique<variable_sensitivity_domain_factoryt>(
        vs_object_factory, vsd_config);
    }
    else if(options.get_bool_option("example"))
    {
      // If you want to pass command line options to domains, you will
      // need to replace ai_domain_factory_default_constructort with
      // your own factory which inherits from
      //  ai_domain_factoryt<example_domaint>
      // and implements the ai_domain_factory_baset::make method
      df = std::make_unique<ai_domain_factory_default_constructort<example_domaint>>();
    }
    
    else if(options.get_bool_option("tvpi"))
    {
      // If you want to pass command line options to domains, you will
      // need to replace ai_domain_factory_default_constructort with
      // your own factory which inherits from
      //  ai_domain_factoryt<example_domaint>
      // and implements the ai_domain_factory_baset::make method
      df = std::make_unique<ai_domain_factory_default_constructort<tvpi_domaint>>();
    }
    
    // non-null is not fully supported, despite the historical options
    // dependency-graph is quite heavily tied to the legacy-ait infrastructure
    // dependency-graph-vs is very similar to dependency-graph

    // Build the storage object
    std::unique_ptr<ai_storage_baset> st = nullptr;
    if(options.get_bool_option("one-domain-per-history"))
    {
      st = std::make_unique<history_sensitive_storaget>();
    }
    else if(options.get_bool_option("one-domain-per-location"))
    {
      st = std::make_unique<location_sensitive_storaget>();
    }

    // Only try to build the abstract interpreter if all the parts have been
    // correctly specified and configured
    if(hf != nullptr && df != nullptr && st != nullptr)
    {
      if(options.get_bool_option("recursive-interprocedural"))
      {
        return std::make_unique<ai_recursive_interproceduralt>(
          std::move(hf), std::move(df), std::move(st), mh);
      }
      else if(options.get_bool_option("three-way-merge"))
      {
        // Only works with VSD
        if(options.get_bool_option("vsd"))
        {
          return std::make_unique<ai_three_way_merget>(
            std::move(hf), std::move(df), std::move(st), mh);
        }
      }
    }
  }
  else if(options.get_bool_option("legacy-ait"))
  {
    if(options.get_bool_option("constants"))
    {
      // constant_propagator_ait derives from ait<constant_propagator_domaint>
      return std::make_unique<constant_propagator_ait>(
        goto_model.goto_functions);
    }
    else if(options.get_bool_option("dependence-graph"))
    {
      return std::make_unique<dependence_grapht>(ns, mh);
    }
    else if(options.get_bool_option("dependence-graph-vs"))
    {
      return std::make_unique<variable_sensitivity_dependence_grapht>(
        goto_model.goto_functions, ns, vs_object_factory, vsd_config, mh);
    }
    else if(options.get_bool_option("vsd"))
    {
      auto df = std::make_unique<variable_sensitivity_domain_factoryt>(
        vs_object_factory, vsd_config);
      return std::make_unique<ait<variable_sensitivity_domaint>>(std::move(df));
    }
    else if(options.get_bool_option("intervals"))
    {
      return std::make_unique<ait<interval_domaint>>();
    }
    else if(options.get_bool_option("example"))
    {
      auto df = std::make_unique<ai_domain_factory_default_constructort<example_domaint>>();
      return std::make_unique<ait<variable_sensitivity_domaint>>(std::move(df));
    }
    else if(options.get_bool_option("tvpi"))
    {
      auto df = std::make_unique<ai_domain_factory_default_constructort<tvpi_domaint>>();
      return std::make_unique<ait<variable_sensitivity_domaint>>(std::move(df));
    }
    
#if 0
    // Not actually implemented, despite the option...
    else if(options.get_bool_option("non-null"))
    {
      return std::make_unique<ait<non_null_domaint> >();
    }
#endif
  }
  else if(options.get_bool_option("legacy-concurrent"))
  {
#if 0
    // Very few domains can work with this interpreter
    // as it requires that changes to the domain are
    // 'non-revertable' and it has merge shared
    if(options.get_bool_option("dependence-graph"))
    {
      return std::make_unique<dependence_grapht>(ns);
    }
#endif
  }

  // Construction failed due to configuration errors
  return nullptr;
}
