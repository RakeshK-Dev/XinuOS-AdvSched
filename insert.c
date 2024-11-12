/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status	insert(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q,		/* ID of queue to use		*/
	  int32		key		/* Key for the inserted process	*/
	)
{
	qid16	curr;			/* Runs through items in a queue*/
	qid16	prev;			/* Holds previous node index	*/

	if (isbadqid(q) || isbadpid(pid)) {
		return SYSERR;
	}

	curr = firstid(q);
	while (queuetab[curr].qkey >= key) {
		curr = queuetab[curr].qnext;
	}

	/* Insert process between curr node and previous node */

	prev = queuetab[curr].qprev;	/* Get index of previous node	*/
	queuetab[pid].qnext = curr;
	queuetab[pid].qprev = prev;
	queuetab[pid].qkey = key;
	queuetab[prev].qnext = pid;
	queuetab[curr].qprev = pid;
	return OK;
}

status insert_mlfq(
	  pid32		pid,		/* ID of process to insert	*/
	  qid16		q       /* ID of queue to use		*/
//	  int32		key		/* Key for the inserted process	*/
)
{
    qid16 tail_index = queuetail(q);
    qid16 prev_index = queuetab[tail_index].qprev;

    /* Link the new process into the queue */
    queuetab[prev_index].qnext = pid;
	queuetab[tail_index].qprev = pid;
	queuetab[pid].qprev = prev_index;
    queuetab[pid].qnext = tail_index;

    return OK;
}

