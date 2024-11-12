/* resched.h */

/* Constants and variables related to deferred rescheduling */

#define DEFER_START     1       /* Start deferred rescehduling          */
#define DEFER_STOP      2       /* Stop  deferred rescehduling          */

#define TIME_ALLOTMENT 10
#define PRIORITY_BOOST_PERIOD 500
#define UPRIORITY_QUEUES 5

/* Structure that collects items related to deferred rescheduling       */

struct  defer   {
        int32   ndefers;        /* Number of outstanding defers         */
        bool8   attempt;        /* Was resched called during the        */
                                /*   deferral period?                   */
};

extern  struct  defer   Defer;
