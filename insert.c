/* insert.c - insert */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  insert  -  Insert a process into a queue in descending key order
 *------------------------------------------------------------------------
 */
status insert(
    pid32  pid,     /* ID of process to insert  */
    qid16  q,       /* ID of queue to use       */
    int32  key      /* Key for the inserted process (number of tickets) */
)
{
    qid16 curr;        /* Runs through items in a queue */
    qid16 prev;        /* Holds previous node index     */

    /* Check if queue ID or process ID is invalid */
    if (isbadqid(q) || isbadpid(pid)) {
        return SYSERR;
    }

    /* Start with the first process in the queue */
    curr = firstid(q);

    /* Traverse through the queue to find the correct insertion point */
    while (queuetab[curr].qkey > key || 
           (queuetab[curr].qkey == key && curr < pid)) {
        curr = queuetab[curr].qnext;
    }

    /* Insert process between the current node and previous node */
    prev = queuetab[curr].qprev;    /* Get index of previous node */
    queuetab[pid].qnext = curr;     /* Insert after prev, before curr */
    queuetab[pid].qprev = prev;
    queuetab[pid].qkey = key;       /* Assign the number of tickets */
    queuetab[prev].qnext = pid;
    queuetab[curr].qprev = pid;

    return OK;
}

