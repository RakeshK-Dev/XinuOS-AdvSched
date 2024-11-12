/* resched.c - resched, resched_cntl */

#include <xinu.h>
#define DEBUG_CTXSW
#define select_process(q)  q = (lsprocess_count() > 1) ? select_lottery() : single_lsprocess();

struct	defer	Defer;

pid32 single_lsprocess() 
{
    pid32 p = firstid(userlist);
    return (proctab[p].tickets > 0) ? getitem(p) : dequeue(readylist);
}

uint32 lsprocess_count() 
{
    if (isempty(userlist)) {
        return 0;  // Return early if the lottery queue is empty
    }

    uint32 pr_count = 0;
    pid32  p = firstid(userlist);

    /* Iterate through the lottery queue and count user processes */
    while (queuetab[p].qnext != EMPTY) {
        if (proctab[p].user_process) {
            pr_count++;
        }
        p = queuetab[p].qnext;  // Move to the next process in the queue
    }
    return pr_count;
}

pid32 select_lottery() 
{
    uint32 tickets = 0;
    pid32 idx = firstid(userlist);
    pid32 tail = queuetail(userlist);

    /* Calculate total tickets while counting the number of entries */
    while (idx != tail) {
        tickets += queuetab[idx].qkey;
        idx = queuetab[idx].qnext;
    }

    /* Pick winner using random selection */
    uint32 ticket_num = rand() % tickets;
    uint32 accumulated_tickets = 0;
    idx = firstid(userlist);  // Reset idx to start of queue

    /* Find winning process */
    while (idx != tail) {
        accumulated_tickets += queuetab[idx].qkey;
        if (accumulated_tickets > ticket_num) {
            break; 
        }
        idx = queuetab[idx].qnext;
    }
	if(idx != tail)
	{
		return getitem(idx);
	}

    /* return the first ready process */
    return dequeue(readylist);
}

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	pid32  priorpid = currpid;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */
	ptold = &proctab[currpid];

	if(ptold->user_process == FALSE)
	{
		if ( ptold->prstate == PR_CURR)
		{
			if (ptold->prprio > firstkey(readylist))
			{
				if (currpid != NULLPROC)
				{
					return;
				}
				else if ((currpid == NULLPROC) && (nonempty(userlist)))
				{
					ptold->prstate = PR_READY;
					insert(currpid, readylist, ptold->prprio);
					select_process(currpid)
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

				if ((firstid(readylist) == NULLPROC) && nonempty(userlist))
				{
					select_process(currpid)
				}
				else
				{
					currpid = dequeue(readylist);
				}
			}
		}
		else
		{
			if ((firstid(readylist) == NULLPROC) && nonempty(userlist))
			{
				select_process(currpid)
			}
			else
			{
				currpid = dequeue(readylist);
			}
		}
	}
	else
	{
		if (ptold->prstate == PR_CURR)
		{
			insert(currpid, userlist, ptold->tickets);
			if (ptold->prprio > firstkey(readylist))
			{
				select_process(currpid)
				if(currpid==priorpid)
				{
					return;
				}
			}
			else
			{
				currpid = dequeue(readylist);
			}
			ptold->prstate = PR_READY;
		}
		else
		{
			if ((firstid(readylist) == NULLPROC) && nonempty(userlist))
			{
				select_process(currpid)
			}
			else
			{
				currpid = dequeue(readylist);
			}
		}
	}

	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	preempt = QUANTUM;		/* Reset time slice for process	*/
	ptnew->num_ctxsw++;

	#ifdef DEBUG_CTXSW
		if(priorpid != currpid)
		{        
			kprintf("ctxsw::%d-%d\n",priorpid ,currpid );
		}
    #endif

	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

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
