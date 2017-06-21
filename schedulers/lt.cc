#include "lt.hh"
#include <iostream>
#include <algorithm>
#include "../options.hh"

using namespace scheduler_simulator;
using namespace std;

SchedulerLT::SchedulerLT (Options* opt) {
  quantum = stoi(opt->get_str("-q"));
  current_quantum = quantum;
}

bool SchedulerLT::is_next() {
  return !ready_list.empty() or 
    (scheduled_proc and scheduled_proc->cpu_time >= 0) or 
    !input_lines.empty();
}

std::shared_ptr<SchedulerLT::Proc> SchedulerLT::schedule_proc() {
  std::shared_ptr<Proc> proc;
  decltype(ready_list.begin()) it;

  while (true) {
    it = max_element(ready_list.begin(), ready_list.end(), [] (auto& a, auto& b) {
        return (a->number_tickets < b->number_tickets);
        });

    proc = *it;
    if (proc->resource != -1) {
      auto r_it = resources_used.find(proc->resource);

      // Trying to access locked resource
      if (r_it != resources_used.end() and r_it->second->id != proc->id) {
        r_it->second->number_tickets += proc->number_tickets;
        r_it->second->borrowed_tickets = proc->number_tickets;
        r_it->second->borrowed_proc = proc;
        proc->number_tickets = 0;

        locked_list.push_back(proc);
        ready_list.erase(it);
      } else {
        break;
      }
    } else {
      break;
    }
  }

  ready_list.erase(it);
  return proc;
}

bool SchedulerLT::schedule() {
  if (time == 0) {
    input_lines.sort([] (auto& a, auto& b) { return stoi(a[1]) < stoi(b[1]); });
  }

  while (is_next_line()) {
    auto in = input_lines.front();
    input_lines.pop_front();

    auto proc = make_shared<Proc>();
    proc->id  = stoi(in[0]);
    proc->cpu_time = stoi(in[2]);
    proc->number_tickets  = stoi(in[3]);

    // Add ticket compensation
    auto it = boost_list.find(proc->id);
    if (it != boost_list.end()) {
      proc->number_tickets *= it->second;
      boost_list.erase(it);
    }

    if (in.size() == 5) {
      proc->resource = stoi(in[4]);
    }

    ready_list.emplace_back(proc);
  }
  bool change_process = !scheduled_proc;

  // If there is already a proccess being executed
  if (scheduled_proc) {
    if (scheduled_proc->cpu_time == 0 or current_quantum == quantum) {

      if (scheduled_proc->cpu_time == 0) {
        cout << time << ": terminate P" << scheduled_proc->id << endl;

        // Free the resources
        if (scheduled_proc->resource != -1) {
          resources_used.erase(scheduled_proc->resource);
          if (scheduled_proc->borrowed_proc) {
            scheduled_proc->borrowed_proc->number_tickets = scheduled_proc->borrowed_tickets;

            //Clean-up
            remove(locked_list.begin(), locked_list.end(), scheduled_proc->borrowed_proc);
            ready_list.push_back(scheduled_proc->borrowed_proc);
          }
        }

        // ticket compensation
        if (current_quantum < quantum) {
          int speedup = quantum / current_quantum;
          boost_list.insert({scheduled_proc->id, speedup});
        }

        scheduled_proc.reset();
        change_process = true;

      } else if (current_quantum == quantum and scheduled_proc->cpu_time > 0) {
        ready_list.push_back(scheduled_proc);
        change_process = true;
      }
    }
  }


  // Find new proccess
  if (change_process and !ready_list.empty()) {
    auto next_proc = schedule_proc();

    // Lock the resources
    if (next_proc->resource != -1)
      resources_used.insert({next_proc->resource, next_proc});

    // Re-schedule new process
    scheduled_proc = next_proc;
    
    cout << time << ": schedule P" << scheduled_proc->id << endl;
    current_quantum = 0;
  }

  if (scheduled_proc) {
    scheduled_proc->cpu_time--;
    current_quantum++;
  }

  time++;

  return true;
}
