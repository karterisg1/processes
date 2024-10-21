#include "list.h"
#include "jobs.h"

void issuejob(char *command, int jobID, int f)
{	
	char *args[64], *job, msg[100];
	int rc, i;
	pid_t pid;
	
	/*save job because string will be tokenized later*/
	job = strdup(command); 
	/*split command in arguments*/
	tokenize(command, args);

	signal(SIGCHLD, handler);
	/*count the running jobs*/
	rc = count(rl);
	/*there is space for running jobs*/
	if (rc < N)
	{
		/*if f==1 function called because some running process ended and a queued will be transfered
        changing the answer the commander will receive*/
		sprintf(msg, "job_%d\t%s\tRUNNING\t", jobID, job);
		if (f == 1)
			send(msg);
		else
			printf("%s\n", msg);
		/*create another process for the job*/
		if ((pid = fork()) < 0) eerror("fork");

		if (pid == 0) 
		{ 
			/*execute the command*/
			execvp(*args, args); 
			eerror(*args);
		}
		else /*add the job to the running ones*/
			rl = insertJob(rl, jobID, job, pid);
	}
	/*no space left, so add the job to the queued ones*/
	else
	{
		sprintf(msg, "job_%d\t%s\tQUEUED\t", jobID, job);
		send(msg);
		ql = insertJob(ql, jobID, job, -1);
	}
}

/*handler for terminated jobs*/
void handler(int sig)
{
	int jobID, status;
	pid_t pid;

	/*find terminated children*/
	/*even if they terminate at the same time*/
	while ((pid = waitpid((pid_t)(-1), &status, WNOHANG)) > 0)
		rl = removeJobpid(rl, pid);
	
	update();
}

/* 	checks if queued jobs can be executed when concurrency changes */
void update()
{
	int rc, qc, i;
	
	rc = count(rl);
	for (i = 0; i < N-rc; i++)
	{
		qc = count(ql);
		if (qc > 0)
		{
			char *job;
			int jobID;
			
			jobID = ql->jobID;
			job = strdup(ql->job);
			ql=updateList(ql, ql);
			issuejob(job, jobID, 0);
		}
	}
}

/*splitting the command into arguments*/
void tokenize(char *s, char **args)
{
	char *tok;
	
	tok=strtok(s, " ");
	while (tok != NULL)
	{
		*args++ = strdup(tok);
		tok=strtok(NULL, " ");
	}
	*args = NULL;
}

void client(char **v)
{
	int fd, fd2, i;
	char text[LIMIT], msg[LIMIT];

	/*open the pipe to read data*/
	if( (fd=open(FIFO1, O_WRONLY)) < 0)
		eerror("Client writing: Error in fifo open");
	strcpy(text, v[1]);
	for(i = 2; v[i] != NULL; i++)
		sprintf(text, "%s %s", text, v[i]);

    /*write the string in the pipe*/  
	if (write(fd, text, LIMIT) == -1)
		eerror("Client: Error in Write");
	close(fd);
	
 	/*open the second pipe for reading response*/
	if ((fd2=open(FIFO2, O_RDWR)) < 0)
		eerror("Client reading: Error in fifo open"); 	
	/*while server is sending something back*/
	while (1) 
	{
		/*read msg*/
		if (read(fd2, msg, LIMIT) < 0)
			eerror("Client: Error in read");
		/* read "end" means the end of arguments*/
		if (strcmp(msg, "end") == 0)
			break;
		printf("%s\n", msg);
	}
	close(fd2);
}

void eerror(char *s)
{
	perror(s);
	exit(1);
}