#include "interactive_scheduler.hh"
#include <iostream>
#include <algorithm>

using namespace scheduler_simulator;

bool InteractiveScheduler::is_next_line() {
  if (input_lines.empty()) 
    return false;

  uint32_t next_time = (uint32_t)atoi(input_lines.front()[1].c_str());

  return (time == next_time);
}
