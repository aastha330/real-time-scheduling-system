# Real-Time Scheduling System

This project demonstrates the impact of priority inversion in real-time systems using a simulated control panel. It explores three scheduling methodologies:
1. **No Protocol**: Simulates unbounded priority inversion where lower-priority tasks can indefinitely delay high-priority tasks.
2. **Priority Inheritance Protocol (PIP)**: Limits priority inversion by making the resource-holding task inherit the priority of the waiting high-priority task.
3. **Priority Ceiling Protocol (PCP)**: Completely avoids priority inversion and deadlocks by assigning priority limits (ceilings) to all resources.

## Requirements
- Python 3.x
- Flask

## Setup Instructions

1. Install the required dependencies:
   ```bash
   pip install -r requirements.txt
   ```
2. Run the application:
   ```bash
   python app.py
   ```
3. Open your browser and navigate to `http://127.0.0.1:5000`

## Features
- **Simulation Viewer**: Terminal-style interface detailing task execution logs and delays.
- **Diagnostics Dashboard**: View exact execution times and simulation analysis.
- **Detailed Comparison**: See a tabulated summary of different protocols.
- **Export Results**: Download simulation logs directly as TXT files for sharing.
This project demonstrates real-time scheduling algorithms.