#pragma once
#include "interactive_scheduler.hh"
#include <map>
#include <queue>
#include <utility>

namespace scheduler_simulator {

class Options;

class SchedulerRR : public InteractiveScheduler {
  public:
    SchedulerRR (Options*);
    virtual ~SchedulerRR () override = default;

    virtual bool schedule() override;
    virtual bool is_next() override;

  private:
    std::queue<std::pair<int, int>> fifo_queue;
    std::pair<int, int> scheduled_proc = {0,0};
    uint32_t quantum = 0;
    uint32_t current_quantum = 0;
};

}
