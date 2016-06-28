#define SENDER_DATA_TYPE 1  //The information type
#define RECV_DONE_TYPE 2 //The done message

//message structure
struct message
{
	//message type
	long mtype;
	
	//how many bytes
	int size;
	
	/*
	//Prints structure
	void print(FILE* fp)
	{
		fprintf(fp, "%ld %d", mtype, size);
	}
	*/
};
