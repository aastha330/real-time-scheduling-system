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
        {"Task L (Low Priority) starts, acquires Lock M.", 0.5},
        {"Task H (High Priority) arrives, preempts Task L.", 0.5},
        {"Task H needs Lock M! Blocks, waiting for Task L...", 0.5},
        {"[ ! ] Task M (Medium Priority) arrives, preempts Task L...", 0.5},
        {"[ ! ] Task M executes... (PRIORITY INVERSION happening)", 1.0},
        {"Task M finishes execution.", 0.5},
        {"Task L resumes, finishes critical section, releases Lock M.", 0.5},
        {"Task H resumes, acquires Lock M, executes.", 0.2},
        {"Task H finishes execution.", 0.2},
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
