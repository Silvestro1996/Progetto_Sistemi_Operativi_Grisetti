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
	/*removing descriptor*/
	sem_des = List_detach(&(running->sem_descriptors), (ListItem*) sem_fd);
	
}
