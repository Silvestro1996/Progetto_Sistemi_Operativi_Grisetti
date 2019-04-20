#include <assert.h>
#include <unistd.h>
#include <stdio.h>
#include "disastrOS.h"
#include "disastrOS_syscalls.h"
#include "disastrOS_semaphore.h"
#include "disastrOS_semdescriptor.h"

void internal_semClose(){
	in sem_fd = running-> syscall_args[0];
	/*checking if it exists*/
	SemDescriptor* sem_des = SemDescriptorList_byFd(&(running->sem_descriptors), sem_fd);
	if (!sem_des){
		disastrOS_debug("There's no semaphore with the given fd\n");
		running->syscall_retvalue = DSOS_NOSEMERROR;
		return;
	}
	/*removing descriptor from Process Control Block*/
	sem_des = List_detach(&(running->sem_descriptors), (ListItem*) sem_fd);
	
	/* <assert> displays an error message on stderr if the expression is FALSE*/
	assert(des); 
	
	Semaphore* Sem = sem_des-> semaphore;
	
	/* removing descriptors from the semaphore*/
	SemDescriptorPtr* sem_des_ptr = List_detach(&(Sem->descriptors, (ListItem*)(sem_des->ptr));
	assert(sem_des_ptr);
	
	/*free*/
	SemDescriptorPtr_free(sem_des_ptr);
	assert(sem_des_ptr);
	SemDescriptor_free(sem_des);
	assert(sem_des);
	
	int dim = sem->descriptors.size;
	
	/*check if the semaphore is not used*/
	if (dim == 0){
		disastrOS_debug("semaphore with id %d will be deleted\n", Sem->id);
		Sem = List_detach(&(semaphore_list), (ListItem*) Sem);
		assert(Sem);
		Semaphore_free(sem);
	}
	
	/*success code*/
	running->syscall_retvalue = 0;
	
}
