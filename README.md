# OS Simulator
We have simulated OS scheduler and OS memory allocation. The scheduler determines an order for the execution of its scheduled processes. It decides which process will run according to a certain data structure and certain memory allocation policy that keeps track of the processes in the system and their status.

## Types of Scheduler
1. First Come First Serve (FCFS).
2. Shortest Job First (SJF).
3. Preemptive Highest Priority First (HPF).
4. Shortest Remaining Time Next (SRTN).
5. Round Robin (RR).

## Types of Memory Allocation
1. First Fit.
2. Next Fit.
3. Best Fit.
4. Buddy System Allocation.

## You can use Makefile to build and run your project
If you added a file to your project add it to the build section in the Makefile. Always start the line with a tab in Makefile, it is its syntax.

### To compile your project, use the command:
```make```

### To run your project, use the command:
```make run```

## Notes
1. headers.h contains clk functions, it should be included anywhere the clock functions are used.
2. You can read more about the project in `Project Phase 1 - Scheduler.pdf` and `Project Phase 2 - Memory.pdf`.

### To get time call:
```getClk();```
