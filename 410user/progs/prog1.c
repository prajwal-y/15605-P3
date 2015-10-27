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

int main()
{
	//int a = 0;
	int pid = fork();
	printf("pid is %d", pid);
	if(pid == 0) {
		while(1) {
			gettid();
			//printf("Hello from CHILD %d\n", pid);
		}
	}
    while (1) {
		gettid();
		//printf("Hello from parent %d\n", pid);
		//a += 2;
		//if (a%100000 == 0) {
			//printf("prog1 %d\n", a);
			//lprintf("prog1 %d\n", a);	
		//}
    }
}
