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

int main()
{
    int tid = gettid();
    lprintf("Thread id is %d", tid);
    while (1) {
    }
}
