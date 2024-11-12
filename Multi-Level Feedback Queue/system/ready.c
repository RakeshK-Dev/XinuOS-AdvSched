/* ready.c - ready */

#include <xinu.h>
#define	nextprocess(q)	(queuetab[q].qnext)

qid16	readylist;			/* Index of ready list		*/

syscall print_ready_list()
{
    pid32 idx = firstid(readylist);
    kprintf("Processes in readylist:\n");

    while (nextprocess(idx) != EMPTY)
    {
        kprintf("%d ", idx);
        idx = nextprocess(idx);
    }

    kprintf("\n");
    return OK;
}

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */
	uint32 queue_index;
	prptr = &proctab[pid];
	prptr->prstate = PR_READY;

	if (prptr->user_process) 
	{
        /* Find the correct multi-level queue based on process priority */
        //uint32 queue_index = UPRIORITY_QUEUES - prptr->prprio;
        
        /* Validate that queue_index is within range before inserting */
        //if (queue_index < UPRIORITY_QUEUES) {
		for (queue_index = 0; queue_index < UPRIORITY_QUEUES; queue_index++)
		{
			if (prptr->prprio == (UPRIORITY_QUEUES - queue_index))
			{
				insert_mlfq(pid, mlfq[queue_index].level);
				break;
			}
        }
    }
	else
	{
		insert(pid, readylist, prptr->prprio);
	}

	resched();

	return OK;
}
