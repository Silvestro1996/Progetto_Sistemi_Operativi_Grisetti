#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){

	int sem_fd = running->syscall_args[0];

	/*checking if it exists*/
	SemDescriptor* sem_des = SemDescriptorList_byFd(&running->sem_descriptors, sem_fd);
	if(!sem_des)
	{
		running->syscall_retvalue = DSOS_NOSEMERROR; //New error code
		return;
	}

	//Get the semaphore associated to the descriptor
	Semaphore* Sem = sem_des->semaphore;

	/*removing descriptor from Process Control Block*/
	sem_des = (SemDescriptor*) List_detach(&running->sem_descriptors, (ListItem*)sem_des);
	/* <assert> displays an error message on stderr if the expression is FALSE*/
	assert(sem_des);

	/* removing descriptors from the semaphore*/
	SemDescriptorPtr* sem_des_ptr = (SemDescriptorPtr*) List_detach(&Sem->descriptors, (ListItem*) sem_des->ptr);
	assert(sem_des_ptr);

	/*free*/
	SemDescriptorPtr_free(sem_des_ptr);
	SemDescriptor_free(sem_des);
	assert(sem_des_ptr || sem_des);

	//int dim = Sem->descriptors.size;
	/*check if the semaphore is not used*/
	if(Sem->descriptors.size == 0 && Sem->waiting_descriptors.size == 0)
	{
		printf("Semaphore with ID %d will be deleted\n", Sem->id);

		Sem = (Semaphore*) List_detach(&semaphores_list, (ListItem*) Sem);
		assert(Sem);
		Semaphore_free(Sem);
	}

	/*success code*/
	running->syscall_retvalue = 0;
	return;
}
