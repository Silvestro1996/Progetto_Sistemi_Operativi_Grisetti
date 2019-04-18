#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semOpen(){
  int semid = running->syscall_args[0];
  
  if (semid <0){
	  disastrOS_debug("The semaphore id is invalid");
	  running->syscall_retvalue = DSOS_SEMNEG;
	  return;
  }
  
}
