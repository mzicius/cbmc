#include "tvpi_bindingt.h"

#include "tvpi_logt.h"

void tvpi_bindingt::set_binding(
  symbol_exprt symbol,
  tvpi_systemt::dimensiont dim)
{
  std::cout << "setting: " << symbol.get_identifier() << " " << dim
            << std::endl;
  if(binding.find(symbol) != binding.end())
  {
    std::cout << "binding found" << std::endl;
    std::cout << "sym: " << symbol.get_identifier()
              << " at dim: " << binding[symbol]
              << " the ref count is: " << references[binding[symbol]]
              << std::endl;
    //fix?
    if(references[binding[symbol]] > 0)
    {
      references[binding[symbol]] = references[binding[symbol]] - 1;
    }
    binding[symbol] = dim;
  }
  else
  {
    std::cout << "binding not found" << std::endl;
    binding.insert(std::make_pair(symbol, dim));
  }
  references[binding[symbol]] = references[binding[symbol]] + 1;
}

tvpi_systemt::dimensiont tvpi_bindingt::lookup_binding(symbol_exprt symbol)
{
  tvpi_systemt::dimensiont result_dim;
  result_dim = (binding.find(symbol) != binding.end()) ? binding[symbol] : -1;
  return result_dim;
}

void tvpi_bindingt::print_binding() const
{
  for(const auto &[symbol, dimension] : binding)
  {
    if(main_tvpi_log.binding_log == log_level::PARTIAL)
    {
      //Skip the __CPROVER variables
      if(dimension > 4)
      {
        std::cout << id2string(symbol.get_identifier()) << "-> " << dimension
                  << std::endl;
      }
    }
  }
}

void tvpi_bindingt::print_references() const
{
  for(const auto &ref : references)
  {
    if(ref.first >= 0)
    {
      //Skip the __CPROVER references
      if(main_tvpi_log.reference_log == log_level::PARTIAL && ref.first>4){
      std::cout << "d" << ref.first << " -> " << ref.second << std::endl;
      }
    }
    else
    {
      std::cout << "-d" << abs(ref.first) << " -> " << ref.second << std::endl;
    }
  }
}

void tvpi_bindingt::add_tmp_ref(tvpi_systemt::dimensiont dim)
{
  std::cout << "the tmp dim ref is: " << dim << std::endl;
  if(references.find(dim) == references.end())
  {
    references.insert(std::make_pair(dim, 0));
  }
}

tvpi_bindingt::binding_map tvpi_bindingt::copy_map() const
{
  return binding;
}

bool tvpi_bindingt::wipe_binding(tvpi_systemt &sys)
{
  bool changed = false;
  for(auto it = references.begin(); it != references.end();)
  {
    const auto &ref = *it;
    if(ref.second == 0)
    {
      std::cerr<<"trying to wipe dim: "<<ref.first<<std::endl;
      sys.existential_project(ref.first);
      it = references.erase(it);
      changed = true;
    }
    else
    {
      ++it;
    }
  }

  return changed;
}

void tvpi_bindingt::override_binding(const binding_map &new_binding)
{
  binding = new_binding;
}
