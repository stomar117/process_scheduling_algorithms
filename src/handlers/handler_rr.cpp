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
    class ProcessHandler: protected nrand::nrand {
        private:
            std::queue<Process> process_queue;
            unsigned int executed_process = 0;
            Process previous_process = Process {"", 0, 0};
            unsigned int waiting_time = 0;
            unsigned int turnaround_time = 0;
            unsigned int quantum = 4;
            std::unordered_set<unsigned int> pid_set;
            std::thread global_process_thread;
            bool shutdown = false;
            void worker ();
        public:
            ProcessHandler () {}
            Process create_process(const std::string, const unsigned int);
            Process create_process(const std::string, const unsigned int, const unsigned int);
            void queue_process(const Process);
            void set_quantum(const unsigned int a) {this->quantum = a;};
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
        this->global_process_thread = std::thread(&ProcessHandler::worker, this);
    };

    void ProcessHandler::worker() {
        while (! this->shutdown) {
            if (this->process_queue.empty()) {continue;}
            Process process = this->process_queue.front();
            for (unsigned int i = 0; i < this->quantum; i++) {
                if (!process.timeout) {
                    break;
                }
                else {
                    process.timeout-=1;
                    sleep(1);
                }
            }
            this->process_queue.pop();
            if (process.timeout) {
                this->process_queue.push(process);
                std::cout << "[-!] process: " << process.name
                          << " with pid: " << process.pid
                          << " paused " << " remaining time: " << process.timeout << '\n';
            }
            else {
                std::cout << "[-!] process: " << process.name
                          << " with pid: " << process.pid
                          << " terminated\n";
            }
        }
        std::cout << "Entering shutdown...\n";
        if (this->process_queue.empty()) {
            std::cout << "Queue cleared...\n";
        }
        else {
            while (! this->process_queue.empty()) {
                Process term = this->process_queue.front();
                std::cout << "Process with pid: " << term.pid << " halted before init, cleared from queue...\n";
                this->process_queue.pop();
            }
        }
        std::cout << "Shutdown complete\n";
    };

    void ProcessHandler::stop_worker() {
        this->shutdown = true;
        this->global_process_thread.join();
    };
}
