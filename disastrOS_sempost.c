#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semPost(){

	int fd = running->syscall_args[0];

	/*taking sem des from running process pcb*/
	SemDescriptor* des = SemDescriptorList_byFd(&running->sem_descriptors, fd);

	if(!des){
		running->syscall_retvalue = DSOS_NOSEMERROR;
		return;
	}
	
	/* incrementing value of the opened semaphore with the given descriptor*/
	Semaphore* sem = des->semaphore;

	sem->count++;
	printf("\nSemaphore #%d has been incremented to %d by process %d\n\n", sem->id, sem->count, running->pid);

	if(sem->count <= 0){ 
		
		//Move the descriptor(its pointer) from list of waiting to list of ready descriptors
		SemDescriptorPtr* sem_des = (SemDescriptorPtr*)List_detach(&sem->waiting_descriptors,
			sem->waiting_descriptors.first);
		List_insert(&sem->descriptors, sem->descriptors.last, (ListItem*)sem_des);

		/* PCB of the process to move to the r. queue*/
		PCB* pcb_to_ready = sem_des->descriptor->pcb;
		pcb_to_ready->status = Ready;

		/*form waiting queue...*/
		List_detach((ListHead*)&waiting_list, (ListItem*)pcb_to_ready);
		/*...to ready queue*/
		List_insert((ListHead*)&ready_list, (ListItem*)ready_list.last, (ListItem*)pcb_to_ready);
		printf("Process #%d has been moved to ready queue\n\n", pcb_to_ready->pid);
	}

	running->syscall_retvalue = 0;
	return;
}
