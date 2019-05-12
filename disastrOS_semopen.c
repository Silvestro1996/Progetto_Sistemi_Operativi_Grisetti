#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"
#include "disastrOS_globals.h"
#include "disastrOS_constants.h"


void internal_semOpen(){
	/*get values from syscall args*/
	int sem_id = running->syscall_args[0];
	int sem_val = running->syscall_args[1];

	/*checking if a semaphore with the given id already exists*/
	Semaphore* mySem = SemaphoreList_byId(&semaphores_list, sem_id); //Need to add sempahore list to globals
	
	if(!mySem)
	{
		printf("It doesnt exist\n");
		 /* it doesn't esist --> let's allocate*/
		mySem = Semaphore_alloc(sem_id, sem_val);
		
		if(!mySem){
			printf("[Allocation failed]\n");
			running->syscall_retvalue = DSOS_SEMALLOCERROR; //New error code
			return;
		}
		/*semaphore in the list*/
		List_insert(&semaphores_list, semaphores_list.last, (ListItem*) mySem);
	}
	
	//Creating the descriptor for the semaphore to add to the running PCB
	int fd = running->last_sem_fd;
	SemDescriptor* semdesc = SemDescriptor_alloc(fd, mySem, running);
	if(!semdesc){
		running->syscall_retvalue = DSOS_ERRCREATEFD; //New error code
		return;
	}
	
	(running->last_sem_fd)++;
	
	/*Creating the pointer to the desriptor*/
	SemDescriptorPtr*  semdesc_ptr = SemDescriptorPtr_alloc(semdesc);
	if(!semdesc_ptr)
	{
		running->syscall_retvalue = DSOS_ERRCREATEFDPTR;
		return;
	}


	/*Adding the descriptor to the list of semaphore descriptors of the process*/
	List_insert(&running->sem_descriptors, running->sem_descriptors.last, (ListItem*)semdesc); 

	/*Insert the pointer into the descriptor*/
	semdesc->ptr = semdesc_ptr;
	
	/*Adding the decriptor pointer to the descriptors list in the semaphore*/
	List_insert(&mySem->descriptors, mySem->descriptors.last, (ListItem*) semdesc_ptr);

	printf("[Process #%d opened semaphore #%d with value %d]\n", running->pid, mySem->id, mySem->count);
	//Return the fd of the semaphore 
	running->syscall_retvalue = semdesc->fd;
}
