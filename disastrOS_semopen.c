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
	  running->syscall_retvalue = DSOS_SEMNEGERROR;
	  return;
  }

  /*checking if a semaphore with the given id already exists*/
  Semaphore* mySem = SemaphoreList_byId(&semaphores_list, semid); 
  
  if (!mySem) {
	  disastrOS_debug("Allocation of semaphore with id: %d\n", semid);
	  /* it doesn't esist --> let's allocate*/
	  mySem = Semaphore_alloc(semid,1);
	  
	  if (mySem == NULL){
		  disastrOS_debug("Allocation failed\n");
		  running->syscall_retvalue = DSOS_SEMALLOCERROR;
		  return;
	  }
	  
	  else{ disastrOS_debug("Successful allocation\n");}
	  
	  /* semaphore in the list*/
	  List_insert(&semaphores_list, semaphore_list.last, (ListItem*) mySem);
  }

		  
		     
}
