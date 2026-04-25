#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void delay(double seconds) {
#ifdef _WIN32
    Sleep((DWORD)(seconds * 1000));
#else
    usleep((useconds_t)(seconds * 1000000));
#endif
}

typedef struct {
    const char* message;
    double delay_sec;
} LogEntry;

void simulate() {
    LogEntry logs[] = {
        {"Initiating Simulation...", 0.5},
        {"Task L starts, acquires Lock M.", 0.5},
        {"Task H arrives, preempts Task L.", 0.4},
        {"Task H needs Lock M! Blocks.", 0.3},
        {"[ * ] Task L INHERITS Task H's High Priority!", 0.2},
        {"Task M arrives, but CANNOT preempt Task L due to inherited priority.", 0.5},
        {"Task L finishes critical section, releases Lock M.", 0.5},
        {"Task L drops back to Low Priority.", 0.2},
        {"Task H resumes, acquires Lock M, executes.", 0.2},
        {"Task H finishes execution.", 0.2},
        {"Task M executes.", 0.3},
        {"Task M finishes execution.", 0.2},
        {"Simulation successfully terminated.", 0.1}
    };
    
    int num_logs = sizeof(logs) / sizeof(logs[0]);
    for (int i = 0; i < num_logs; i++) {
        delay(logs[i].delay_sec);
        printf("%s\n", logs[i].message);
        fflush(stdout);
    }
}

int main() {
    simulate();
    return 0;
}
