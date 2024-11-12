/* ready.c - ready */

#include <xinu.h>
#define	nextprocess(q)	(queuetab[q].qnext)

qid16	readylist;			/* Index of ready list		*/
qid16   userlist;

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

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;

	if (prptr->user_process == TRUE)
	{
		insert(pid, userlist, prptr->tickets);
	}
	else
	{
		insert(pid, readylist, prptr->prprio);
	}

	resched();

	return OK;
}
