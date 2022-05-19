#include <string>

typedef struct process_properties {
public:
    std::string name;
    unsigned int pid;
    unsigned int timeout;
    unsigned int priority;
} Process;

struct ProcPriorityComparator {
    bool operator() (const Process &proc_1, const Process &proc_2) {
        return (proc_1.priority > proc_2.priority);
    }
};

struct ProcTimeComparator {
    bool operator() (const Process &proc_1, const Process &proc_2) {
        return (proc_1.timeout > proc_2.timeout);
    }
};
