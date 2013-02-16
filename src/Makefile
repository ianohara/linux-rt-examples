default: all

all: scheduler_syscalls hrtimer_test

scheduler_syscalls: schedulerTest.o
	gcc -o schedulerTest schedulerTest.o

hrtimer_test: hrtimerTest.o
	gcc -o hrtimerTest -lrt hrtimerTest.o

clean:
	rm *.o schedulerTest
