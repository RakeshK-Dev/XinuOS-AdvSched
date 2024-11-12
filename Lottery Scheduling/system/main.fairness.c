#include <xinu.h>
#include <stdio.h>

void timed_execution(uint32 runtime) {
    while (proctab[currpid].runtime < runtime);
}

int main() 
{
    uint32 i = 1, j;
    pid32 pr1, pr2;
    float fairness; // Variable to store fairness ratio
    uint32 runtime_values[] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
    uint32 num_points = sizeof(runtime_values) / sizeof(runtime_values[0]);

    // Loop through defined runtime values
    for (j = 0; j < num_points; j++) {
        uint32 current_runtime = runtime_values[j];

        pr1 = create_user_process(timed_execution, 1024, "timed_execution", 1, current_runtime);
        pr2 = create_user_process(timed_execution, 1024, "timed_execution", 1, current_runtime);

        set_tickets(pr1, 50);
        set_tickets(pr2, 50);

        resume(pr1);
        resume(pr2);

        receive();    
        receive();    

        sleepms(50); 

        // Calculate fairness ratio
        fairness = (float)proctab[pr1].turnaroundtime / proctab[pr2].turnaroundtime;

        // Print execution data
        kprintf("Runtime: %d, pr1 runtime: %d, pr1 turnaround time: %d, pr2 runtime: %d, pr2 turnaround time: %d\n", 
                 current_runtime, proctab[pr1].runtime, proctab[pr1].turnaroundtime, 
                 proctab[pr2].runtime, proctab[pr2].turnaroundtime);
        
        kprintf("Fairness F = %f\n\n", fairness);
    }

    return OK;
}
