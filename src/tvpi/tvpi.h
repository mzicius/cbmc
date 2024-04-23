#ifndef TVPI_H
#define TVPI_H

#include "complete.h"
#include "../analyses/ai_domain.h"

#include <vector>

class tvpi_domaint : public ai_domain_baset{

public:

tvpi_domaint();

static int result_call; 
static std::chrono::milliseconds total_duration;
static std::size_t input_sys_size;
static std::size_t output_sys_size;

virtual ~tvpi_domaint();

  /// Transform updates the domain with the effect of the instruction
  /// given by "from".
  /// As GOTO has multiple next instructions, the abstract trace "to"
  /// is passed so you can tell whether it is the branch taken or not.
 virtual void transform(
    const irep_idt &function_from,
    trace_ptrt from,
    const irep_idt &function_to,
    trace_ptrt to,
    ai_baset &ai,
    const namespacet &ns) override;

/// Merges two domains together
/// \return true if and only if *this has been modified / extended
/// If it is true then it will requeue this domain for analysis
bool merge(const tvpi_domaint &b, trace_ptrt, trace_ptrt);

/// Set the domain to be empty, i.e. representing nothing
void make_bottom() override;

/// Set the domain to allow all possibilities
void make_top() override;

/// Set up the domain for the start of the program
void make_entry() override;

/// Is the domain bottom or not
bool is_bottom() const override;

/// Is the domain top or not
bool is_top() const override;

/// Output the domain as a string
void output(std::ostream &out, const ai_baset &ai, const namespacet &ns)
    const override;

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

private:
static std::vector<std::shared_ptr<inequality>> sys;

protected:
  // Domain state goes here
  // For now we will track which states may (an overapproximation) be reachable.
  bool may_reach;

};

#endif
