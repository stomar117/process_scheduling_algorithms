#include <iostream>
#include <string>
#include <random>

#define FCFS

#ifdef FCFS
#include "handlers/handler_fcfs.cpp"
#else
#ifdef SJF
#include "handlers/handler_sjf.cpp"
#else
#ifdef ROUND
#include "handlers/handler_rr.cpp"
#else
#ifdef PRIORITY
#include "handlers/handler_ps.cpp"
#endif
#endif
#endif
#endif

using phandler::ProcessHandler;

/**
- This is just to randomlu generate a pro_runtime from 1s to 5s
- This function's existance is solely for demostration purpose
- This function is not supposed to be used in an actual circumstance,
  since the process creator should have an estimate for the process runtime
  the process may take.
**/
unsigned int randtime() {
    std::random_device seed;
    std::mt19937_64 generator(seed());
    std::uniform_int_distribution<int> range(1, 9);
    return range(generator);
}

unsigned int randpriority() {
    std::random_device seed;
    std::mt19937_64 generator(seed());
    std::uniform_int_distribution<> range(0, 3);
    return range(generator);
}

int main(int argc, char *argv[]) {
    std::cout << '\n';
    ProcessHandler * proc_handler = new ProcessHandler();
    proc_handler->init_worker();
    for (int i = 0; i < 20; i++) {
        proc_handler->queue_process(
            proc_handler->create_process(
                "random",
                randtime()
            )
        );
    }
    std::cin.get();
    proc_handler->stop_worker();
    proc_handler->get_turnaround_time();
    proc_handler->average_waiting_time();
    return 0;
}
