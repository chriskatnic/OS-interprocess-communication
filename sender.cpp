#include <string.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "msg.h"    /* For the message struct */

/* The size of the shared memory chunk */
#define SHARED_MEMORY_CHUNK_SIZE 1000

/* The ids for the shared memory segment and the message queue */
int shmid, msqid;

/* The pointer to the shared memory */
char* sharedMemPtr;

/**
 * Sets up the shared memory segment and message queue
 * @param shmid - the id of the allocated shared memory 
 * @param msqid - the id of the shared memory
 */

void init(int& shmid, int& msqid, char*& sharedMemPtr)
{	

	printf("Creating Key...\n");
	key_t key = ftok("keyfile.txt", 'a');
	if (key == -1)
	{
		perror("ftok");
		exit(1);
	}
	printf("Key created...\n");
	

	shmid = shmget(key, SHARED_MEMORY_CHUNK_SIZE, 0666 | IPC_CREAT);
	if (shmid == -1)
	{
		perror("shmget");
		exit(1);
	}

	/* TODO: Attach to the shared memory */
	sharedMemPtr = (char*)shmat(shmid, (void *)0, 0);
	if (sharedMemPtr == (char*) -1)
	{
		perror("shmat");
		exit(1);
	}

	/* TODO: Attach to the message queue */
	msqid = msgget(key, 0666 | IPC_CREAT);
	if (msqid == -1)
	{
		perror("msgget");
		exit(1);
	}
	

	/* Store the IDs and the pointer to the shared memory region in the corresponding parameters */

	

	printf("Working as intended!\n");
}


/**
 * Performs the cleanup functions
 * @param sharedMemPtr - the pointer to the shared memory
 * @param shmid - the id of the shared memory segment
 * @param msqid - the id of the message queue
 */
void cleanUp(const int& shmid, const int& msqid, char* sharedMemPtr)
{

	printf("Detatching...\n");
	shmdt(sharedMemPtr);
	printf("Success!\n");
}


/**
 * The main send function
 * @param fileName - the name of the file
 */
void send(const char* fileName)
{

	FILE* fp = fopen(fileName, "r");
	message sndMsg;
	message rcvMsg;
	
	/* Was the file open? */
	if(!fp)
	{
		perror("fopen");
		exit(-1);
	}
	
	printf("File exists and opened...\n");
	while(!feof(fp))
	{
		
		if((sndMsg.size = fread(sharedMemPtr, sizeof(char), SHARED_MEMORY_CHUNK_SIZE, fp)) < 0)
		{
			perror("fread");
			exit(-1);
		}

		printf("Starting sender program...\n");

		/* TODO: Send a message to the receiver telling him that the data is ready 
		 * (message of type SENDER_DATA_TYPE) 
		 */
		sndMsg.mtype = SENDER_DATA_TYPE;
		printf("We are here...\n");

		printf("Sending message...\n");
		if(msgsnd(msqid, &sndMsg, sizeof(sndMsg.size), 0) == -1)
		{
			perror("msgsnd");
		}
		printf("Sent message!\n");
		
		/* TODO: Wait until the receiver sends us a message of type RECV_DONE_TYPE telling us 
		 * that he finished saving the memory chunk. 
		 */
		printf("Recieving message...\n");
		if(msgrcv(msqid, &rcvMsg, sizeof (rcvMsg.size), RECV_DONE_TYPE, 0) == -1)
		{
			perror("msgrcv");
			exit(1);
		}
		printf("Received message!\n");
	}
	

	sndMsg.size = 0;
	sndMsg.mtype = SENDER_DATA_TYPE;
	printf("Sending nothing...\n");
	if(msgsnd(msqid, &sndMsg, sizeof(sndMsg.size), 0) == -1)
	{
		perror("msgsnd");
	}
	printf("Sent nothing!\n");
		
	/* Close the file */
	fclose(fp);
	printf("Closed file.\n");
}

int main(int argc, char** argv)
{
	
	/* Check the command line arguments */
	if(argc < 2)
	{
		fprintf(stderr, "USAGE: %s <FILE NAME>\n", argv[0]);
		exit(-1);
	}
		
	/* Connect to shared memory and the message queue */
	init(shmid, msqid, sharedMemPtr);
	
	/* Send the file */
	send(argv[1]);
	
	/* Cleanup */
	cleanUp(shmid, msqid, sharedMemPtr);
		
	return 0;
}
