#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

/*decrementa valore semaforo. Se < 0, entra nella coda di attesa e ritorna un codice errore*/
void internal_semWait(){
  int fd = running->syscall_args[0];
  /*keeping track of the running process for later*/
  PCB* old_one = running;
  
  /*taking sem from running process pcb*/
  SemDescriptor* des = SemDescriptorList_byFd(&(running->sem_descriptors), fd);
  
  
  if (!des){
		disastrOS_debug("There's no semaphore with the given fd\n");
		running->syscall_retvalue = DSOS_NOSEMERROR;
		return;
  }
  /* decrementing value of the open semaphore with the given descriptor*/
  Semaphore* sem = des->semaphore;
   if(!sem){
    running->syscall_retvalue = DSOS_NOSEMERROR;
  }
  sem->count--;
  disastrOS_debug("semaphore with id %d ha been decreased...count: %d\n", sem->id, sem->count); 
  
  /*if count <0 right into waiting queue*/
  
  
  if (sem->count <0){
	  running->status = Waiting;
	  
	  SemDescriptorPtr* des_ptr = SemDescriptorPtr_alloc(des);
	  /*descrptor in waiting queue*/
	  List_insert(&(sem->waiting_descriptors), 
					sem->waiting_descriptors.last, 
					(ListItem*) des_ptr);
	  /*process in waiting queue*/		
	  List_insert(&waiting_list, waiting_list.last, (ListItem*) des->pcb);
		
      /*next process*/
      PCB* next_one = (PCB*) List_detach(&(ready_list), ready_list.first);
      running = next_one;
      disastrOS_debug("process:%d is now running\n",next_one->pid); 
		
	  }
	  
  old_one->syscall_retvalue = 0;
  return;
}
