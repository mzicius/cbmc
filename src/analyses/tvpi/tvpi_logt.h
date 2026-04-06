#pragma once

enum class log_level{OFF,PARTIAL,FULL};

struct tvpi_log_flags{

log_level system_log = log_level::PARTIAL;
log_level projection_log = log_level::OFF;
log_level result_log = log_level::OFF;
log_level binding_log = log_level::PARTIAL;
log_level reference_log = log_level::PARTIAL;
log_level CPROVER_log = log_level::OFF;
log_level convex_log = log_level::FULL;


};

inline tvpi_log_flags main_tvpi_log;