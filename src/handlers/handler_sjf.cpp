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
#include <vector>
#include "../utils/nrand.cpp"
#include "../headers/process_model.h"
namespace phandler {
    class ProcessHandler: protected nrand::nrand {
        private:
            std::priority_queue<
                Process,
                std::vector<Process>,
                ProcTimeComparator
            > process_queue;
            unsigned int executed_process = 0;
            Process previous_process = Process {"", 0, 0};
            unsigned int waiting_time = 0;
            unsigned int turnaround_time = 0;
            unsigned int quantum = 4;
            std::unordered_set<unsigned int> pid_set;
            std::thread global_process_thread;
            bool init_check = true;
            bool shutdown = false;
            std::vector<Process> request_vector;
            void worker ();
        public:
            ProcessHandler () {}
            Process create_process(
                const std::string,
                const unsigned int
            );
            Process create_process(
                const std::string,
                const unsigned int,
                const unsigned int
            );
            void queue_process(Process);
            void set_quantum(const unsigned int a) {
                this->quantum = a;
            };
            void init_worker();
            void stop_worker();
            unsigned int get_turnaround_time();
            unsigned int average_waiting_time();
    };

    Process ProcessHandler::create_process(
        const std::string name,
        const unsigned int timeout
    ) {
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

    Process ProcessHandler::create_process(
        const std::string proc_name,
        const unsigned int timeout,
        const unsigned int priority
    ) {
        unsigned int pid = 0;
        while (this->pid_set.find(pid) == pid_set.end()) {
            pid = randint(0, 10000);
            if (this->pid_set.find(pid) == this->pid_set.end()) {
                pid_set.insert(pid);
                break;
            }
        }
        return Process {
            proc_name,
            pid,
            timeout,
            priority
        };
    }

    void ProcessHandler::queue_process(Process process) {
        this->process_queue.push(process);
        std::cout << "[--] process with pid: "
            << process.pid
            << " queued...\n";
        sleep(1);
    };

    void ProcessHandler::init_worker() {
        this->global_process_thread = std::thread(
            &ProcessHandler::worker,
            this
        );
    };

    void ProcessHandler::worker() {
        while (true) {
            if (this->shutdown) {
                std::cout << "Entering shutdown...\n";
                if (this->process_queue.empty()) {
                    std::cout << "Queue cleared, shutdown complete\n";
                    break;
                }
                else {
                    while (! this->process_queue.empty()) {
                        Process term_proc = this->process_queue.top();
                        std::cout << "process pid: " << term_proc.pid
                            << " cleared from queue\n";
                        this->process_queue.pop();
                    }
                    std::cout << "Queue cleared, shutdown complete\n";
                    break;
                }
            }
            if (!this->process_queue.empty()) {
                Process process = this->process_queue.top();
                if (&process != &this->previous_process) {
                    this->waiting_time += this->previous_process.timeout;
                    this->previous_process = process;
                }
                std::cout << "[!-] running " << process.name
                          << " with pid: " << process.pid
                          << " requested time: " << process.timeout << '\n';

                this->process_queue.pop();
                if (this->pid_set.find(process.pid) != this->pid_set.end()) {
                    this->pid_set.erase(process.pid);
                    sleep(process.timeout);
                    this->turnaround_time+=process.timeout;
                    std::cout << "[-!] process " << process.name
                              << " with pid: " << process.pid
                              << " terminated\n";
                }
                else {
                    std::cout << "[!!] process pid: \'"
                    << process.pid
                    << "\' is invalid.\n";
                }
            }
        }
    };

    void ProcessHandler::stop_worker() {
        this->shutdown = true;
        this->global_process_thread.join();
    }
};
