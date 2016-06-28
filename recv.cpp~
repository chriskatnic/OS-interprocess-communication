#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include "msg.h"    /* For the message struct */


/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
void *sharedMemPtr;

/* The name of the received file */
const char recvFileName[] = "recvfile";


/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 * @param sharedMemPtr - the pointer to the shared memory
 */

void init(int& shmid, int& msqid, void*& sharedMemPtr)
{

	printf("starting init method...\n");
	key_t key = ftok("keyfile.txt", 'a');
	if(key == -1)
	{
		perror("ftok");
		exit(1);
	}
	printf("getting key...\n");
	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT);
	if (shmid == -1)
	{
		perror("shmget");
		exit(1);
	}

	printf("key retrieved.\ngetting shared memory address...\n");
	sharedMemPtr = shmat(shmid, (void *)0, 0);
	if (sharedMemPtr == (void *) -1)
	{
		perror("shmat");
		exit(1);
	}
	printf("shared memory retrieved at: %i\n", shmid);
	msqid = msgget(key, 0666 | IPC_CREAT);
	if (msqid == -1)
	{
		perror("msgget");
		exit(1);
	}
	
	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */
	printf("initialization complete...\n");
	
}
 

/**
 * The main loop
 */
void mainLoop()
{
	/* The size of the mesage */
	int msgSize = 0;
	
	//open file, check for existence
	FILE* fp = fopen(recvFileName, "w");
	if(!fp)
	{
		perror("fopen");	
		exit(-1);
	}
	printf("opened file to write bytestream...\n");
	//declaration of message variables for sending and recieving
	//msgsize "initialized" to 1 
	message sndMsg;
	message rcvMsg;
	msgSize = 0;
	


	printf("waiting to recieve message...");
	//Wait for the first message, get the size		
	if (msgrcv(msqid,&rcvMsg, sizeof(rcvMsg.size), SENDER_DATA_TYPE, 0) == -1)
	{
		perror("msgrcv");
		exit (1);
	}
	
	msgSize = rcvMsg.size;



	//
	//as long as there is a message in the queue, continue recieving data through stream
	//first save message to sharedmemory, and check for errors
	//
	while(msgSize != 0){	
		if(fwrite(sharedMemPtr, sizeof(char), msgSize, fp) < 0){
			perror("fwrite");}
		sndMsg.mtype = RECV_DONE_TYPE;
		sndMsg.size = 0;
		if(msgsnd(msqid, &sndMsg, 0, 0) == -1){
			perror("msgsnd");}
		if (msgrcv(msqid,&rcvMsg, sizeof(rcvMsg.size), SENDER_DATA_TYPE, 0) == -1)
			{
				perror("msgrcv");
				exit (1);
			}
			msgSize = rcvMsg.size;
			}
	fclose(fp);
}



//
//	cleanUp called when queues and shared memory need to be deleted
//	shmid and msqid are pointers to location of shared memory chunk and message queue
//
void cleanUp(const int& shmid, const int& msqid, void* sharedMemPtr)
{
	
	if( shmdt(sharedMemPtr) != -1){
		shmctl(shmid, IPC_RMID, NULL);
		msgctl(msqid, IPC_RMID, NULL);
		printf("Shared memory and message queue deleted.\n");}
}
void ctrlCSignal(int signal)
{
	//Call cleanUp when ctrlC is pressed (called in main function)
	cleanUp(shmid, msqid, sharedMemPtr);
}

int main(int argc, char** argv)
{
	
	//SIGINT is the signal sent to the process when ctrl-C is pressed.
	signal(SIGINT, ctrlCSignal);
	
	//initialize shared memory chunk and message queue
	init(shmid, msqid, sharedMemPtr);
	
	/* Go to the main loop */
	mainLoop();

	//cleanup at the end of program in case ctrl-c was not pressed
	cleanUp(shmid, msqid, sharedMemPtr);
		
	return 0;
}
