#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

/*Incrementa valore semaforo. Se era == 0, va in esecuzione. Ritorna 0 se successo, altrimenti un codice errore*/
void internal_semPost(){
	int fd = running->syscall_args[0];
	
	/*taking sem from running process pcb*/
	SemDescriptor* des = SemDescriptorList_byFd(&(running->sem_descriptors), fd);
	
	if (!des){
		disastrOS_debug("There's no semaphore with the given fd\n");
		running->syscall_retvalue = DSOS_NOSEMERROR;
		return;
	}
	
	/* incrementing value of the opened semaphore with the given descriptor*/
    Semaphore* sem = des->semaphore;
    if(!sem){
    running->syscall_retvalue = DSOS_NOSEMERROR;
	}
	
	sem->count++;
	disastrOS_debug("semaphore with id %d ha been increased...count: %d\n", sem->id, sem->count);
	
	if (sem->count <=0){
	  
	  SemDescriptorPtr* sem_des = (SemDescriptorPtr*)List_detach(&(sem->waiting_descriptors),
										sem->waiting_descriptors.first); 
	  List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*)sem_des);
	  
	  /* PCB of the process to move to tre r. queue*/
	  PCB* pcb_to_ready = sem_des->descriptor->pcb;
	  pcb_to_ready->status = Ready;
	  /*form waiting queue...*/
	  List_detach((ListHead*)&waiting_list, (ListItem*)pcb_to_ready);
	  /*...to ready queue*/
	  List_insert((ListHead*)&ready_list, (ListItem*)ready_list.last, (ListItem*)pcb_to_ready);
	  disastrOS_debug("\nProcess #%d moved to ready queue\n", pcb_to_ready->pid);

	  }
	  
	  running->syscall_retvalue = 0;
	  return;
	  
	  
	
	
}
