/*
 * Examples of using all of the scheduling system calls.
 *  A nice summary of all of the scheduling system calls can
 *  be found at:
 *      http://www.makelinux.net/books/ulk3/understandlk-CHP-7-SECT-6#understandlk-CHP-7-SECT-6
 *
 * System Calls Exemplified Here:
 *   sched_get_priority_max()
 *   sched_get_priority_min()
 *   sched_getparam()
 * Ian O'Hara
 * ianohara at gmail dot com
 * October 25, 2012
 */

#include <stdio.h> /* For printf(...)*/
#include <stdlib.h> /* For exit() */
#include <errno.h> /* For the errno definition and all of the error state macros */

#include <sched.h> /* for all SCHED_/sched_ calls */
#include <sys/resource.h> /* for getpriority() */

/* Defines _POSIX_PRIORITY_SCHEDULING if this system has all of
 *     the scheduling system calls
 *
 * Also has pid_t which is a signed integer type used for storing
 * system process ids (pids).  pid_t = 0 normally refers to the
 * current process.
 */
#include <unistd.h>

#ifdef _POSIX_PRIORITY_SCHEDULING
static const int have_sched_prio = 1;
#else
static const int have_sched_prio = 0;
#endif /* _POSIX_PRIORITY_SCHEDULING */

/* For mapping sched_policy integer -> corresponding string name.  The #defines
 * can be found by looking in /usr/include/sched.h and following #includes
 * until you find the SCHED_* definitions.
 *
 * These are for OS X 10.6.8 and I found the defines
 * in: /usr/include/pthread_impl.h
 */
const static int SCHED_POLICY_MAX = 4;
char *sched_policies[] = {
    "0_NOT_A_POLICY",
    "SCHED_OTHER",
    "SCHED_RR",
    "3_NOT_A_POLICY",
    "SCHED_FIFO"
};

void
print_process_prio(pid_t pid)
{
    /* Get the proirity of this process. This is the NON-REALTIME priority.
     *   http://linux.die.net/man/2/getpriority
     */
    /* This can return -1 as a legitimate value, so first clear errno
     * and make sure to check it after the call.
     */
    errno = 0;
    int this_prio = getpriority(PRIO_PROCESS, pid);
    if ((this_prio == -1) && (errno)) {
        perror("Syscall getpriority failed");
    } else {
        printf("Process priority is: %i\n", this_prio);
    }
}

void
print_sched_type(pid_t pid)
{
    int policy = sched_getscheduler(pid);
    if (policy < 0) {
        perror("Syscall sched_getscheduler() had problems");
        return;
    }
    if (policy > SCHED_POLICY_MAX) {
        printf("Syscall sched_getscheduler() returned a policy number greater than allowed!\n");
        return;
    }
    printf("The current scheduling policy is: %s\n", sched_policies[policy]);
}

void
print_sched_priority(pid_t pid)
{
    /* Get and inspect the scheduling parameters.  This can contain realtime relevant
     * information if this process is operating with SCHED_FIFO
     */
    struct sched_param param;
    if (sched_getparam(pid, &param) < 0) {
        perror("Syscall sched_getparam barfed");
    } else {
        printf("The sched_param schedule priority is: %i\n", param.sched_priority);
    }
}

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

    print_process_prio(0);
    print_sched_priority(0);
    print_sched_type(0);
}


