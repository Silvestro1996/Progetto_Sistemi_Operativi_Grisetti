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
  ListHead semaphoress = semaphores_list;
  Semaphore* mySem = SemaphoreList_byId(&semaphoress, semid); 
  
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
	  List_insert(&semaphores_list, semaphores_list.last, (ListItem*) mySem);
  }
  
  ListHead opened_ones = running->sem_descriptors;
  
  SemDescriptor* op_des = Search_sem_id(&opened_ones, semid);
  
  if (op_des){
	  running->syscall_retvalue = op_des->fd;
	  return;
  }
  
  else{
	  (running->last_sem_fd)++;
	  
	  int fd = running->last_sem_fd;
	  SemDescriptor* semdesc = SemDescriptor_alloc(fd,mySem,running);
	  if(!semdesc){
		  running->syscall_retvalue = DSOS_ERRCREATEFD;
		  return;
	  }
	  
	  disastrOS_debug("Descriptor created\n");
	  
	  SemDescriptorPtr* semdesc_ptr = SemDescriptorPtr_alloc(semdesc);
	  if(!semdesc_ptr){
		  running->syscall_retvalue = DSOS_ERRCREATEFDPTR; // no creation of sem dscriptor pointer
		  return;
	  }
	  
	  semdesc->ptr = semdesc_ptr; //put into the struct
	  List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)semdesc);
	  List_insert(&mySem->descriptors, mySem->descriptors.last, (ListItem*)semdesc_ptr); /* fill the lists */
	  
	  running->syscall_retvalue = fd;
		  
	  }
		     
}
