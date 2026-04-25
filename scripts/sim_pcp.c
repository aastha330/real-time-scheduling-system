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
        {"Task L starts, acquires Lock M.", 0.3},
        {"[ * ] System applies Priority Ceiling dynamically.", 0.2},
        {"Task H arrives...", 0.3},
        {"Task H realizes Lock M is held, bounded blocking starts.", 0.2},
        {"Task L finishes critical section immediately under ceiling protection.", 0.5},
        {"Task L releases Lock M.", 0.2},
        {"Task H acquires Lock M, executes.", 0.2},
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
