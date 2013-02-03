/*
 * Exemplifies how to check if hrtimers are available,
 * and then if they are how to use them.
 */

#include <stdio.h> /* for printf */
#include <time.h> /* for clock_getres, clock_gettime */

/* This is taken from cyclictest.c in the rt-tests suite.
 * (Comments added by me)
 */
static int have_hrtimers(void)
{
    struct timespec ts;

    /* Check the resolution of the Monotonic clock, which
     * counts up monotonically from system boot.
     *
     * Should return 0 if there are no problems with the clock.
     */
    if (clock_getres(CLOCK_MONOTONIC, &ts)) {
        perror("check_timer clock_getres problem");
        return 0;
    }

    /* If the monotonic clock has 1 nanosecond resolution, then
     * the kernel is using hrtimers.
     */
    return !(ts.tv_sec != 0 || ts.ts.tv_nsec != 1);
}

int main(int argc, char *argv[])
{
    if (!have_hrtimers()) {
        printf("Error: hrtimers are not enabled, cannot start hrtimerTest!");
        exit(1);
    }

    /* Create a timer that triggers every 100us which calls a function that prints
     * information every 1s (so we don't get hung on io trying to print something
     * every 100us)
     */


    

}


