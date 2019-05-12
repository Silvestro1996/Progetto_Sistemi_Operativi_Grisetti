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

	/*taking sem descriptor from running process pcb*/
	SemDescriptor* des = SemDescriptorList_byFd(&running->sem_descriptors, fd);
;
	if(!des)
	{
		running->syscall_retvalue = DSOS_NOSEMERROR;
		return;
	}

	 /* decrementing value of the open semaphore with the given descriptor*/
	Semaphore* sem = des->semaphore;
	
	sem->count--;
	printf("\nSemaphore #%d has been decreased to %d by process %d\n\n", sem->id, sem->count, running->pid);


	if(sem->count < 0)
	{ 

		/*Moving the descriptor pointer from the ready list of descriptors to the waiting one*/
		List_detach(&sem->descriptors, (ListItem*) des->ptr);
		List_insert(&sem->waiting_descriptors, sem->waiting_descriptors.last, (ListItem*) des->ptr);

		/*Change the process status and insert it in the list of waiting processes*/
		running->status = Waiting;
		List_insert(&waiting_list, waiting_list.last, (ListItem*) running);
		printf("Process #%d has been moved to the waiting queue\n\n", running->pid);

		/*next process*/
		PCB* next_one = (PCB*)List_detach(&ready_list, (ListItem*)ready_list.first);
		running = next_one;
		printf("Process #%d is now running \n\n", next_one->pid);
	}
	

	running->syscall_retvalue=0;
	return;
}
