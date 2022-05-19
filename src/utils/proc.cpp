#include <vector>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <string>
#include <thread>
#include <iostream>
#include <queue>
#include <random>
#include <unordered_set>
#include "../utils/nrand.cpp"
#include "../headers/process_model.h"

namespace phandler {
    class ProcessHandler : protected nrand::nrand {
        private:
            std::queue<Process> process_queue;
            std::priority_queue<Process, std::vector<Process>, ProcPriorityComparator> process_queue_p;
            std::priority_queue<Process, std::vector<Process>, ProcTimeComparator> process_queue_t;
            int executed_processes = 0;
            Process previous_process = Process {"0", 0, 0};
            unsigned int waiting_time = 0;
            unsigned int turnaround_time = 0;
            std::unordered_set<unsigned int> pid_set;
            void worker ();
            std::thread t1;
            bool shutdown = false;
        public:
            ProcessHandler () {};
            Process create_process(const std::string, const unsigned int);
            Process create_process(const std::string, const unsigned int, const unsigned int);
            void queue_process(Process);
            void init_worker();
            void stop_worker();
            unsigned int get_turnaround_time();
            unsigned int average_waiting_time();
    };

    Process ProcessHandler::create_process(const std::string name, const unsigned int timeout) {
        unsigned int pid = 0;
        while (this->pid_set.find(pid) == pid_set.end()) {
            pid = randint(0, 10000);
            if (this->pid_set.find(pid) == this->pid_set.end()) {
                pid_set.insert(pid);
                break;
            }
        }
        return Process {name, pid, timeout, randint(0, 3)};
    };

    Process ProcessHandler::create_process(const std::string proc_name, const unsigned int timeout, const unsigned int priority) {
        unsigned int pid = 0;
        while (this->pid_set.find(pid) == pid_set.end()) {
            pid = randint(0, 10000);
            if (this->pid_set.find(pid) == this->pid_set.end()) {
                pid_set.insert(pid);
                break;
            }
        }
        return Process {proc_name, pid, timeout, priority};
    }

    void ProcessHandler::queue_process(const Process process) {
        this->process_queue.push(process);
        std::cout << "[--] process with pid: " << process.pid << " queued...\n";
        sleep(1);
    };

    void ProcessHandler::init_worker() {
        t1 = std::thread(&ProcessHandler::worker, this);
    };

    void ProcessHandler::stop_worker() {
        this->shutdown = true;
        this->t1.join();
    };

    unsigned int ProcessHandler::get_turnaround_time(){
        return this->turnaround_time;
    };

    unsigned int ProcessHandler::average_waiting_time(){
        return (float)this->waiting_time/(float)(
            this->executed_processes == 0 ? 1: this->executed_processes
                                                );
    }
};
