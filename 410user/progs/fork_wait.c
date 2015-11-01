/** @file 410user/progs/fork_wait.c
 *  @author zra
 *  @brief Tests fork() and wait().
 *  @public yes
 *  @for p3
 *  @covers fork wait set_status vanish
 *  @status done
 */

#include <syscall.h>
#include <stdlib.h>
#include "410_tests.h"
#include <report.h>
#include <simics.h>

DEF_TEST_NAME("fork_wait:");

int main()
{
  int pid, status;

  report_start(START_CMPLT);
  pid = fork();
  lprintf("pid in for_exec is %d", pid);

  if (pid < 0) {
    report_end(END_FAIL);
    exit(-1);
  }
  
  if (pid == 0) {
    exit(17);
    lprintf("should not come herw");
    report_end(END_FAIL);   /* panic("vanish() didn't"); */
  }

  if (wait(&status) != pid) {
    lprintf("wait != pid ?");
    report_end(END_FAIL);
    exit(-1);
  }

  if (status != 17) {
    lprintf("status si != 17 is %d", status);
    report_end(END_FAIL);
    exit(-1);
  }

  report_end(END_SUCCESS);
  exit(0);
}
