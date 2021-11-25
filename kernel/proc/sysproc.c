#include "../inc/types.h"
#include "../inc/defs.h"

int
sys_fork(void)
{
  return fork();
}