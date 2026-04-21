import time
import sys

def simulate():
    logs = [
        ("Initiating Simulation...", 0.5),
        ("Task L (Low Priority) starts, acquires Lock M.", 0.5),
        ("Task H (High Priority) arrives, preempts Task L.", 0.5),
        ("Task H needs Lock M! Blocks, waiting for Task L...", 0.5),
        ("[ ! ] Task M (Medium Priority) arrives, preempts Task L...", 0.5),
        ("[ ! ] Task M executes... (PRIORITY INVERSION happening)", 1.0),
        ("Task M finishes execution.", 0.5),
        ("Task L resumes, finishes critical section, releases Lock M.", 0.5),
        ("Task H resumes, acquires Lock M, executes.", 0.2),
        ("Task H finishes execution.", 0.2),
        ("Simulation successfully terminated.", 0.1)
    ]
    
    for message, delay in logs:
        time.sleep(delay)
        print(message)
        sys.stdout.flush()

if __name__ == "__main__":
    simulate()
