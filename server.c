#include "list.h"
#include "jobs.h"

list rl = NULL;		/* List of running jobs */
list ql = NULL;		/* List of queued jobs */
int N = 1;			/* Concurrency level */
int LIMIT = 512+1;	/* size usable for read/write */

/* Server procedure */
void server() 
{
	int jobID = 0;
	
	/*make fifo to send response back to commander*/
	if (mkfifo(FIFO2, 0666) == -1 && errno != EEXIST)
		eerror ("mkfifo failed");
	
	while(1) 
	{
		int fd; 			/* Server file descriptor */
		int i;
		char text[LIMIT];
		char *op; 			/* operation's name */
		char *par; 			/* operation's parameters */
		char *args[64]; 	/* vector of triples (jobId, job name, queue number) */
		char msg[100];		/* pipe messages */
		
		for (i=0; i<64; i++)
			args[i] = NULL;

		if ((fd=open(FIFO1, O_RDWR)) < 0)
			eerror("Server: Error in fifo open"); 

		if (read(fd, text, LIMIT) < 0)
			eerror("Server: Error in read");
		
		close(fd);

		/* split the string. First token is operation and second is the command */
		op = strtok(text, " ");
	
		/*action is operation driven */
		if (strcmp(op, "issueJob") == 0) 
		{
			par = strtok(NULL, "");
			if (par == NULL)
				printf("\"issueJob\" : Too few argument\n");
			else 
			{
				jobID++;
				issuejob(par, jobID, 1);
			}
		}
		else if (strcmp(op, "setConcurrency") == 0)
		{
			par = strtok(NULL, " ");
			N = atoi(par);
			sprintf(msg, "Concurrency changed to: %d", N);
			send(msg);
			update(); /* Update lists */
		}
		else if (strcmp(op, "stop") == 0)
		{
			int id; /* Number of job to be stopped */
			int f;  /* check if job runs or queued */
			
			par = strtok(NULL, " ");
			sscanf(par,"job_%d", &id);
			
			rl = stop(rl, id, &f);
			if (f == 0) /* job is not running*/
			{
				ql = stop(ql, id, &f);
				if (f == 0) /* job is not queued*/
				{
					strcpy(msg, "Unknown job");
				}
				else /* job is queued */
					sprintf(msg, "job_%d removed", id);
			}
			else /* job is running*/
				sprintf(msg, "job_%d terminated", id);
			send(msg);
		}
		else if (strcmp(op, "poll") == 0)
		{
			par = strtok(NULL, " ");

			if (strncmp(par, "running", 7) == 0)
			{
				print(rl, args);
				sendv(args);
			}
			else if (strncmp(par, "queued", 6) == 0)
			{
				print(ql, args);
				sendv(args);
			}
			else
				printf("\"poll\" running|queued\n");
			for(i = 0; args[i] != NULL; i++)
			{
				free(args[i]);
				args[i] = NULL;
			}
		}
		else if (strncmp(op, "exit", 4) == 0)
			exit_op(fd);
		else
			send("Unknown Operation\n");
		fflush(stdout);	
	}
}

/*write msg to commander*/
void send(char *msg) 
{
	int fd2;

	if( (fd2=open(FIFO2, O_WRONLY)) < 0) 
		eerror("Server writing: Error in fifo open");
	
	if (write(fd2, msg, LIMIT) == -1)
		eerror("server: Error in Write");
	
	if (write(fd2, "end", LIMIT) == -1)
		eerror("Server: Error in Write");
	
	close(fd2);
}

/*write vector of messages to commander*/
void sendv(char **args)
{
	int fd2, i;

	if((fd2=open(FIFO2, O_WRONLY)) < 0)
		eerror("Commander writing: Error in fifo open");
	for (i = 0; args[i] != NULL; i++)
	{
		if (write(fd2, args[i], LIMIT) == -1)
			eerror("Commander: Error in Write");
	}
	if (write(fd2, "end", LIMIT) == -1)
		eerror("Commander: Error in Write");
	
	close(fd2);
}

/* Terminating server */
void exit_op()
{
	rl = stopAll(rl);		/* Terminate all running jobs */
	unlink(server_file);	/* deleting server filename */
	send("jobExecutorServer terminated.");
	unlink(FIFO1);			/* deleting named fifo 1 */
	unlink(FIFO2);			/* deleting named fifo 2 */
	exit(1);
}