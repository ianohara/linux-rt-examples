default: all

all: scheduler_syscalls

scheduler_syscalls: schedulerTest.o
	gcc -o schedulerTest schedulerTest.o
	rm *.o

clean:
	rm *.o schedulerTest
