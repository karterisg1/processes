#include "jobs.h"

int main(int argc, char *argv[]) 
{
	pid_t pid;	/* Server pid */
	FILE *fp;	/* Server file pointer */

	fp = fopen(server_file, "r");
	if (fp == NULL) 
	{
		/* File not exist, so create the server */
		pid = fork();
		if (pid == -1) eerror("Fork failed");
		if (pid == 0) 
		{
	        /*server: write pid to file*/
			fp = fopen(server_file, "w");
			fprintf(fp, "%d\n", getpid());
			fclose(fp);
			server();
		}
		else 
		{
            /*create fifo - named pipe*/
			if (mkfifo(FIFO1, 0666) == -1 && errno != EEXIST)
				eerror("mkfifo failed");
		}
	}
	else /*file exists so just close it */
		fclose(fp);
	if (argc == 1) 
		eerror("Two few arguments.\n");
	client(argv);
}