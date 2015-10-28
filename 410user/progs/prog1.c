/** @file 410user/progs/idle.c
 *  @author ?
 *  @brief Idle program.
 *  @public yes
 *  @for p2 p3
 *  @covers
 *  @status done
 */
#include <simics.h>
#include <syscall.h>
#include <stdio.h>

void print_tid(int i) {
	int tid = gettid();
	if (i == 0) {
		printf("Hello from PARENT %d\n", tid);
	} else if (i == 1) {
		printf("Hello from CHILD %d\n", tid);
	}
}

int main()
{
	int a = 0;
	int pid = fork();
	printf("pid is %d\n", pid);
	if(pid == 0) {
		int b = 0;
		printf("pid in child is %d tid: %d\n", pid, gettid());
		while(1) {
			b++;
			if(b%100000 == 0) {
				print_tid(1);
			}
		}
	}
	else {
		printf("pid in parent is %d tid: %d\n", pid, gettid());
    	while (1) {
			a++;
			if(a%100000 == 0) {
				print_tid(0);
			}
    	}
	}
}
