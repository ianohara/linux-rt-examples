/* Copyright 2013 Ian O'Hara
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
 * Exemplifies how to check if hrtimers are available,
 * and then if they are how to use them.
 * 
 *
 * Note: A lot of the "rt" idioms were found in the rt-tests source code.
 *
 */

#include <stdio.h> /* for printf */
#include <time.h> /* for clock_getres, clock_gettime */
#include <signal.h> /* sigevent, sigaction */

static const unsigned long USEC_PER_SEC = 1000000u;

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
    return !(ts.tv_sec != 0 || ts.tv_nsec != 1);
}

/* Keep track of the number of times the timer has triggered. */
static unsigned long trigger_cnt = 0;
static void sig_alarm_handler(int sig) {
    trigger_cnt++;
}

int main(int argc, char *argv[])
{
    if (!have_hrtimers()) {
        printf("Error: hrtimers are not enabled/available, cannot start hrtimerTest!");
        return 1;
    } else {
        printf("--> hrtimers are available.\n");
    }

    /* Create a timer that triggers every 100us which calls a function that prints
     * information every 1s (so we don't get hung on io trying to print something
     * every 100us)
     *
     * The timer_create manpage is useful (it has a source code example):
     *   http://www.kernel.org/doc/man-pages/online/pages/man2/timer_create.2.html
     */
    unsigned long interval_us = 1000;
    printf("Creating a timer that triggers every %u microseconds.\n", interval_us);
    struct timespec interval = {
        .tv_sec = interval_us / USEC_PER_SEC,
        .tv_nsec = (interval_us % USEC_PER_SEC) * 1000
    }
    timer_t timer_id;
    struct itimerspec timer_spec;
    struct sigevent sigevt;
    sigset_t sig_mask;
    struct sigaction sigact;

    /*
     * This is the bare bones example, so no threads are used.  If multiple
     * threads are needed, then the linux specific SIGEV_THREAD_ID value for
     * the sigev_notify field of the struct sigevent passed to timer_create
     * can be used to have the timer trigger signal get sent to a specific
     * thread within the process.
     */

    sigevt.sigev_notify = SIGEV_SIGNAL;
    sigevt.sigev_signo = SIG_ALARM;

     /* Do nothing special, just call the specified handler when the signal is received. */
    sigact.sa_handler = &sig_alarm_handler;
    sigact.sa_flags = 0;
    sigemptyset(&sigact.sa_mask);

    if (sigaction(sigevt.sigev_signo, &sigact, NULL)) {
        perror("sigaction: ");
        return 1;
    }
    
    if (timer_create(CLOCK_MONOTONIC, &sigevt, &timer_id)) {
        perror("timer_create: ");
        return 1;
    }

    /* After the timer fires, the interval to set for the next
     * firing of the timer.  If zero, this timer only fires once.
     */
    timer_spec.it_interval = interval;
    /* When to trigger for the first time, by
     * default relative to time of call to timer_settime */
    timer_spec.it_value = interval; 

    if (timer_settime(timer_id, 0, &timer_spec, NULL)) {
        perror("timer_settime: ");
        return 1;
    }
     
    printf("Now we wait...\n");
    unsigned int sleep_int = 2;
    unsigned int total_time = 0;
    for (;;) {
        sleep(sleep_int);
        total_time += sleep_int;
        print("After %u seconds the timer has triggered %lu times.", total_time, trigger_cnt);
    }
}


