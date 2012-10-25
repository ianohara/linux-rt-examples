/*
 * Test out all of the scheduling system calls.  
 *  A nice summary of all of the scheduling system calls can
 *  be found at:
 *      http://www.makelinux.net/books/ulk3/understandlk-CHP-7-SECT-6#understandlk-CHP-7-SECT-6
 *
 * Ian O'Hara
 * ianohara at gmail dot com
 * October 25, 2012
 */

#include <stdio.h> /* For printf(...) */
#include <stdlib.h> /* For exit() */

#include <sched.h> /* for all SCHED_/sched_ calls */
#include <unistd.h> /* Defines _POSIX_PRIORITY_SCHEDULING if this system has all of the scheduling system calls */

#ifdef _POSIX_PRIORITY_SCHEDULING
static const int have_sched_prio = 1;
#else
static const int have_sched_prio = 0;
#endif /* _POSIX_PRIORITY_SCHEDULING */

int
main(int argc, char *argv[])
{
    if (!have_sched_prio) {
        printf("This system does not have POSIX process priority scheduling!\n");
        exit(1);
    }

    /* Get the maximum schedule priority
        http://linux.die.net/man/2/sched_get_priority_max
    */
    int max_prio_FIFO = sched_get_priority_max(SCHED_FIFO);
    int max_prio_RR = sched_get_priority_max(SCHED_RR);
    int max_prio_OTHER = sched_get_priority_max(SCHED_OTHER);

    printf("Scheduling Priority Maximums:\n"
           "    SCHED_FIFO: %i\n"
           "    SCHED_RR: %i\n"
           "    SCHED_OTHER: %i\n",
           max_prio_FIFO,
           max_prio_RR,
           max_prio_OTHER);

    /* Get the minimum schedule priorities for each process scheduling type
     *  http://linux.die.net/man/2/sched_get_priority_min
     */
    int min_prio_FIFO = sched_get_priority_min(SCHED_FIFO);
    int min_prio_RR = sched_get_priority_min(SCHED_RR);
    int min_prio_OTHER = sched_get_priority_min(SCHED_OTHER);

    printf("Scheduling Priority Minimums:\n"
           "    SCHED_FIFO: %i\n"
           "    SCHED_RR: %i\n"
           "    SCHED_OTHER: %i\n",
           min_prio_FIFO,
           min_prio_RR,
           min_prio_OTHER);
}

