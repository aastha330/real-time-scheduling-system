import time
import sys

def simulate():
    logs = [
        ("Initiating Simulation...", 0.5),
        ("Task L starts, acquires Lock M.", 0.5),
        ("Task H arrives, preempts Task L.", 0.4),
        ("Task H needs Lock M! Blocks.", 0.3),
        ("[ * ] Task L INHERITS Task H's High Priority!", 0.2),
        ("Task M arrives, but CANNOT preempt Task L due to inherited priority.", 0.5),
        ("Task L finishes critical section, releases Lock M.", 0.5),
        ("Task L drops back to Low Priority.", 0.2),
        ("Task H resumes, acquires Lock M, executes.", 0.2),
        ("Task H finishes execution.", 0.2),
        ("Task M executes.", 0.3),
        ("Task M finishes execution.", 0.2),
        ("Simulation successfully terminated.", 0.1)
    ]
    
    for message, delay in logs:
        time.sleep(delay)
        print(message)
        sys.stdout.flush()

if __name__ == "__main__":
    simulate()
