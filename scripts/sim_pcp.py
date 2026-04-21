import time
import sys

def simulate():
    logs = [
        ("Initiating Simulation...", 0.5),
        ("Task L starts, acquires Lock M.", 0.3),
        ("[ * ] System applies Priority Ceiling dynamically.", 0.2),
        ("Task H arrives...", 0.3),
        ("Task H realizes Lock M is held, bounded blocking starts.", 0.2),
        ("Task L finishes critical section immediately under ceiling protection.", 0.5),
        ("Task L releases Lock M.", 0.2),
        ("Task H acquires Lock M, executes.", 0.2),
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
