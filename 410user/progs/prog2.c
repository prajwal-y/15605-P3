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
	int a = 0;
    while (1) {
		a+= 2;
		if(a%200000 == 0) {
			//printf("This is prog2 %d\n", a);
			//lprintf("This is prog2 %d\n", a);
		}
    }
}
