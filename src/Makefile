default: all

all: scheduler_syscalls

scheduler_syscalls: schedulerTest.o
	gcc -o schedulerTest schedulerTest.o

hrtimer_test: hrtimerTest.o
	gcc -o hrtimerTest hrtimerTest.o

clean:
	rm *.o schedulerTest
