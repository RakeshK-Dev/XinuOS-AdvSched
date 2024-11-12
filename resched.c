/* resched.c - resched, resched_cntl */

#include <xinu.h>
//#define DEBUG_CTXSW

struct	defer	Defer;

void reset_timing()
{
	ctr1000 = 0;
	priority_update = 0;
}

uint32 mlfq_process_count(qid16 q) 
{
    uint32 pr_count = 0;
    pid32  p = firstid(q);

    while (queuetab[p].qnext != EMPTY) {
        if (proctab[p].user_process) {
            pr_count++;
        }
        p = queuetab[p].qnext;
    }
    return pr_count;
}

uint32 mlfq_get_level(uint32 priority) 
{
	uint32 i;
    for (i = 0; i < UPRIORITY_QUEUES; i++) 
    {
        if (mlfq[i].priority == priority) 
        {
            return mlfq[i].level;
        }
    }

    return mlfq[0].level;
}

pid32 mlfq_scheduling()
{
    pid32 pid;
	uint32 i;

    if (priority_update >= PRIORITY_BOOST_PERIOD)
    {
		qid16 queue;
    	pid32 p, tail, next_pid;
        priority_update = 0;

        for (i = 0; i < UPRIORITY_QUEUES; i++)
        {
            queue = mlfq[i].level;
            p = firstid(queue);
            tail = queuetail(queue);

            while (p != tail)
            {
				proctab[p].prprio = mlfq[0].priority;
				proctab[p].process_assigned = 0;
				proctab[p].upgrades++;
                
                next_pid = queuetab[p].qnext;

                if (i > 0)
                {
                    dequeue(queue);
                    insert_mlfq(p, mlfq[0].level);
                }
                
                p = next_pid;
            }
        }

        p = firstid(sleepq);
        tail = queuetail(sleepq);

        while (p != tail)
        {
            if (proctab[p].user_process)
            {
                proctab[p].prprio = mlfq[0].priority;
				proctab[p].process_assigned = 0;
                proctab[p].upgrades++;
            }

            p = queuetab[p].qnext;
        }
    }

    for (i = 0; i < UPRIORITY_QUEUES; i++)
    {
        if (nonempty(mlfq[i].level))
        {
            pid = dequeue(mlfq[i].level);
            break;
        }
    }

    return pid;
}

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void resched(void) /* Assumes interrupts are disabled */
{
	struct procent *ptold; /* Ptr to table entry for old process */
	struct procent *ptnew; /* Ptr to table entry for new process */
	pid32 priorpid = currpid;
	uint32 i = 0;

	/* If rescheduling is deferred, record attempt and return */
	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	ptold = &proctab[currpid]; /* Point to process table entry for the current (old) process */
	uint32 total_queue_count = 0;

	if (ptold->user_process == FALSE) 
	{
		if (ptold->prstate == PR_CURR) 
		{
			if (ptold->prprio > firstkey(readylist)) 
			{
				for (i = 0; i < UPRIORITY_QUEUES; i++) {
					total_queue_count += mlfq_process_count(mlfq[i].level);
				}

				if (currpid == NULLPROC && total_queue_count > 0) {
				ptold->prstate = PR_READY;
				insert(currpid, readylist, ptold->prprio);
				currpid = mlfq_scheduling();
				}
				else
				{
					return;
				}
			} 
			else 
			{
				ptold->prstate = PR_READY;
				insert(currpid, readylist, ptold->prprio);
				currpid = dequeue(readylist);
			}
		} 
		else 
		{
			for (i = 0; i < UPRIORITY_QUEUES; i++) {
				total_queue_count += mlfq_process_count(mlfq[i].level);
			}

			/* Schedule from MLFQ if no process in readylist */
			currpid = (firstid(readylist) == NULLPROC && total_queue_count > 0) ? mlfq_scheduling() : dequeue(readylist);
		}
	} 
	else 
	{
		/* Handling for user process */
		if (ptold->prstate == PR_CURR) 
		{
			if (ptold->prprio > firstkey(readylist)) 
			{
				/* Perform MLFQ priority update only when necessary */
				uint32 i;
				for (i = 0; i < UPRIORITY_QUEUES; i++) 
				{
					if (mlfq[i].priority == ptold->prprio) {
						if (ptold->process_assigned >= mlfq[i].time_assigned && i < (UPRIORITY_QUEUES - 1)) {
							ptold->prprio = mlfq[i + 1].priority;
							ptold->process_assigned = 0;
							ptold->downgrades++;
						}
						insert_mlfq(currpid, mlfq[i].level);
						break;
					}
				}
				currpid = mlfq_scheduling();
				if (currpid == priorpid) return;
			} 
			else 
			{
				insert_mlfq(currpid, mlfq_get_level(ptold->prprio));
				currpid = dequeue(readylist);
			}
			ptold->prstate = PR_READY;
		} 
		else 
		{
			uint32 i;
			for (i = 0; i < UPRIORITY_QUEUES; i++) 
			{
				if (mlfq[i].priority == ptold->prprio) 
				{
					if (ptold->process_assigned >= mlfq[i].time_assigned) 
					{
						if (i < UPRIORITY_QUEUES - 1) 
						{
							ptold->prprio = mlfq[i + 1].priority;
							ptold->downgrades++;
						}
						ptold->process_assigned = 0;
					}
					break;
				}
			}

			total_queue_count = 0;
			for (i = 0; i < UPRIORITY_QUEUES; i++) {
				total_queue_count += mlfq_process_count(mlfq[i].level);
			}

			currpid = (firstid(readylist) == NULLPROC && total_queue_count > 0) ? mlfq_scheduling() : dequeue(readylist);
		}
	}

	/* Switching to new process */
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM; /* Reset time slice for process */
	ptnew->num_ctxsw++;

	#ifdef DEBUG_CTXSW
	if (priorpid != currpid) {
		kprintf("\nctxsw::%d-%d\n", priorpid, currpid);
	}
	#endif

	ctxsw(&ptold->prstkptr, &ptnew->prstkptr); /* Context switch */

	return;
}


/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
