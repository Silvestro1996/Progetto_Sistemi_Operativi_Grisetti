#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include "disastrOS.h"
#include "disastrOS_globals.h"
#include <time.h> // for time

#define BUFFER_SIZE 32
#define FILL 0
#define EMPTY 1
#define RW 2
#define PRODS 10
#define CONS  10
#define ITERATIONS 2

#define COL(x) "\033[" #x ";1m"
#define COL_RED COL(31)
#define COL_GREEN COL(32)
#define COL_WHITE COL(37)
#define COL_GRAY "\033[0m"

#define ERROR_HANDLER(condition, ret_msg)               \
	do                                                  \
	{                                                   \
		if (condition && (running->pid != 0))           \
		{                                               \
			printf("%s: %d \n", ret_msg, running->pid); \
			disastrOS_exit(disastrOS_getpid() + 1);     \
		}                                               \
		else if (condition && (running->pid == 0))      \
		{                                               \
			printf("%s:", ret_msg);                     \
			disastrOS_exit(disastrOS_getpid() + 1);     \
		}                                               \
	} while (0)

int buffer[BUFFER_SIZE];
void producer(void *param);
void consumer(void *param);
time_t currentTime; // system time
int insPointer = 0, remPointer = 0;


void sleeperFunction(void *args)
{
	printf("Hello, I am the sleeper, and I sleep %d\n", disastrOS_getpid());
	while (1)
	{
		getc(stdin);
		disastrOS_printStatus();
	}
}

void getTime()
{
	currentTime = time(NULL); // get the system time (Day Month Date HH:MM:SS Year)
}

int myRand()
{
	return rand() % 999;
}

int init_buffer()
{
	int i;
	for (i=0; i< BUFFER_SIZE; i++)
		buffer[i] = -1;
	return 0;
}

int print_buffer()
{

	int i;
	for (i=0; i< BUFFER_SIZE; i++)
		printf("%sSlot %d: 	%d%s\n",COL_WHITE, i, buffer[i],COL_GRAY);
	printf("\n");	
	return 0;
}


int insert_item(int item)
{
	int test = buffer[insPointer];
	if (test == -1){
		buffer[insPointer] = item;
		insPointer = (insPointer + 1) % BUFFER_SIZE;
	}
	return 0;
}

int remove_item()
{
	int item = buffer[remPointer];
	if (item!=-1){
		buffer[remPointer] = -1;
		remPointer = (remPointer + 1) % BUFFER_SIZE;
	}
	return item;
}

void producer(void *param)
{
	int ret;
	int i = 0;

	int sem_fill= disastrOS_semOpen(FILL, 0);
    ERROR_HANDLER(sem_fill < 0,"Error semOpen sem_fill process ");

    int sem_empty = disastrOS_semOpen(EMPTY, BUFFER_SIZE);
    ERROR_HANDLER(sem_empty < 0,"Error semOpen sem_empty process ");
	
	int sem_rw = disastrOS_semOpen(RW, 1);
	ERROR_HANDLER(sem_rw <0 , "Error semOpen sem_rw process ");

    printf("Semaphores initialized...\n");

	disastrOS_sleep(10);

	getTime();

	while (i<ITERATIONS)
	{
		//sem_waits
		ret = disastrOS_semWait(sem_empty);
		ERROR_HANDLER(ret != 0, "Error semWait sem_empty process ");

		ret = disastrOS_semWait(sem_rw);
		ERROR_HANDLER(ret != 0, "Error semWait sem_mutex process ");
		int random = myRand();
		printf("%sProducer inserts %d at time %s  (Process ID: %d)\n", COL_GREEN, random, ctime(&currentTime), running->pid); // Modified to show date and time  and the thread ID.
		insert_item(random);
		printf("%sAdded element %d %s\n", COL_GREEN, random, COL_GRAY);
		printf("%sBuffer:%s\n",COL_WHITE, COL_GRAY);
		print_buffer();
		//sem_posts
		ret = disastrOS_semPost(sem_rw);
		ERROR_HANDLER(ret != 0, "Error semPost sem_mutex process ");

		ret = disastrOS_semPost(sem_fill);
		ERROR_HANDLER(ret != 0, "Error semPost sem_fill process ");
	
		i++;
	}

	ret = disastrOS_semClose(sem_fill);
	ERROR_HANDLER(ret != 0, "Error semClose sem_fill process ");
    
	ret = disastrOS_semClose(sem_empty);
	ERROR_HANDLER(ret != 0, "Error semClose sem_empty process ");

	ret = disastrOS_semClose(sem_rw);
	ERROR_HANDLER(ret != 0, "Error semClose sem_rw process ");


	printf("Semaphores closed...\n");
	disastrOS_exit(disastrOS_getpid() + 1);
}

void consumer(void *param)
{
	int ret;
	int i = 0;
	disastrOS_sleep(10);
	getTime();
	
	int sem_fill= disastrOS_semOpen(FILL, 0);
    ERROR_HANDLER(sem_fill < 0,"Error semOpen sem_fill process ");

    int sem_empty = disastrOS_semOpen(EMPTY, BUFFER_SIZE);
    ERROR_HANDLER(sem_empty < 0,"Error semOpen sem_empty process ");
	
	int sem_rw = disastrOS_semOpen(RW, 1);
	ERROR_HANDLER(sem_rw <0 , "Error semOpen sem_rw process ");

    printf("Semaphores initialized...\n");

	while (i < ITERATIONS)
	{
		//sem_waits
		ret = disastrOS_semWait(sem_fill);
		ERROR_HANDLER(ret != 0, "Error semWait sem_fill process ");

		ret = disastrOS_semWait(sem_rw);
		ERROR_HANDLER(ret != 0, "Error semWait sem_rw process ");
		printf("%sConsumer consumes at time %s  (Process ID: %d))\n", COL_RED, ctime(&currentTime), running->pid); // Modified to show date and time and the thread ID.
		int item = remove_item();
		printf("Removed element %d %s\n", item, COL_GRAY);
		printf("%sBuffer:%s\n",COL_WHITE, COL_GRAY);
		print_buffer();
		//sem_posts
		ret = disastrOS_semPost(sem_rw);
		ERROR_HANDLER(ret != 0, "Error semPost sem_rw process ");

		ret = disastrOS_semPost(sem_empty);
		ERROR_HANDLER(ret != 0, "Error semPost sem_empty process ");
		i++;
	}


	ret = disastrOS_semClose(sem_fill);
	ERROR_HANDLER(ret != 0, "Error semClose sem_fill process ");
    
	ret = disastrOS_semClose(sem_empty);
	ERROR_HANDLER(ret != 0, "Error semClose sem_empty process ");

	ret = disastrOS_semClose(sem_rw);
	ERROR_HANDLER(ret != 0, "Error semClose sem_rw process ");


	printf("Semaphores closed...\n");
	disastrOS_exit(disastrOS_getpid() + 1);
}

void CoreFunction()
{

	disastrOS_printStatus();
	printf("hello, I am init and I just started pid=%d\n", running->pid);
	disastrOS_spawn(sleeperFunction, 0);
	
	int sem_fill= disastrOS_semOpen(FILL, 0);
    ERROR_HANDLER(sem_fill < 0,"Error semOpen sem_fill process ");

    int sem_empty = disastrOS_semOpen(EMPTY, BUFFER_SIZE);
    ERROR_HANDLER(sem_empty < 0,"Error semOpen sem_empty process ");
	
	int sem_rw = disastrOS_semOpen(RW, 1);
	ERROR_HANDLER(sem_rw <0 , "Error semOpen sem_rw process ");

    printf("Semaphores initialized...\n");

	printf("I feel like to spawn %d nice processes\n", PRODS+CONS);
	int children = 0;
	int i=0, ret;
	int fd[PRODS+CONS];
	int prod_done=0, cons_done=0;
	
	for (i = 0; i < PRODS+CONS; i++)
		fd[i] = -1;
	
	i = 0;
	for (; i < PRODS+CONS; ++i)
	{
		int type = 0;
		int mode = DSOS_CREATE;
		printf("mode: %d\n", mode);
		printf("opening resource\n");
		fd[i] = disastrOS_openResource(i, type, mode);
		printf("fd=%d\n", fd[i]);
		if (prod_done < PRODS && i%2 == 0)
		{
			disastrOS_spawn(producer,0);
			prod_done++;
		}
		else if (cons_done < CONS && i%2 != 0)
		{
			disastrOS_spawn(consumer,0);
			cons_done++;
		}		
		else if (cons_done == CONS && prod_done < PRODS)
		{
			disastrOS_spawn(producer,0);
				prod_done++;
		}

		children++;
	}

	printf("TOTAL CHILDREN:%d\n", children);
	int retval;
	int pid;
	while (children > 0 && (pid = disastrOS_wait(0, &retval)) >= 0)
	{

		printf("initFunction, child: %d terminated, retval:%d, alive: %d \n",
			   pid, retval, children);
		--children;
	}

	for (i = 0; i <PRODS+CONS; ++i)
	{
		printf("closing resource %d\n", fd[i]);
		disastrOS_closeResource(fd[i]);
		disastrOS_destroyResource(i);
	}

	ret = disastrOS_semClose(sem_fill);
	ERROR_HANDLER(ret != 0, "Error semClose sem_fill process ");
    
	ret = disastrOS_semClose(sem_empty);
	ERROR_HANDLER(ret != 0, "Error semClose sem_empty process ");

	ret = disastrOS_semClose(sem_rw);
	ERROR_HANDLER(ret != 0, "Error semClose sem_rw process ");


	printf("Semaphores closed...\n");
	disastrOS_printStatus();

	printf("shutdown!\n");
	disastrOS_shutdown();
}

int main(int argc, char *argv[])
{
	char *logfilename = 0;
	if (argc > 1)
	{
		logfilename = argv[1];
	}

	if (CONS > PRODS)
	{
		printf("CONS > PRODS....exit....\n");
		return 0;
	}
	init_buffer();
	printf("start\n");
	disastrOS_start(CoreFunction, 0, logfilename);
	return 0;
}
