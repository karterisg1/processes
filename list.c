#include "list.h"
#include "jobs.h"

/*count jobs*/
int count(list head) 
{
	int c = 0;
	list p;
	
	for (p = head; p != NULL; p=p->next)
		c++;
	return c;
}

/* Creates a node with new job */
list newJob(int jobID, char *job, int pid)
{
	list t;
	
	t = (list) malloc(sizeof(*t));
	if (t == NULL)
	{
		perror("Insufficient memory...\n");
		exit(1);
	}
	t->jobID = jobID;
	strcpy(t->job, job);
	t->pid = pid;
	t->next = NULL;
	return t;
}

/*add job to a list*/
list insertJob(list head, int jobID, char *job, int pid)
{	
	list p, t, q = NULL;
	
	for (p = head; p != NULL; p=p->next)
		q = p;
		
	t = newJob(jobID, job, pid);
	if (q == NULL)
		head = t;
	else
		q->next = t;
	return head;
}

/*print jobs*/
void print(list head, char **args) 
{
	int i;
	char s[LIMIT];
	list p;
	
	for (i = 0, p = head; p != NULL; p=p->next)
	{
		sprintf(s, "job_%d\t%s\t%d", p->jobID, p->job, i+1);
		args[i++] = strdup(s);
	}
	args[i] = NULL;
}


/*locate Job in the list of jobs*/
list locateJob(list head, int jobID) 
{	
	list p;

	for (p = head; p != NULL; p=p->next)
	{
		if (p->jobID == jobID)
			break;
	}
	return p;
}

/*locate Job in the list of jobs by pid*/
list locateJobpid(list head, int pid) 
{	
	list p;

	for (p = head; p != NULL; p=p->next)
	{
		if (p->pid == pid)
			break;
	}
	return p;
}


/* returns list of jobs excluding node t*/
list updateList(list head, list t) 
{	
	list p, q = NULL;

	if (t != NULL)
	{
		for (p = head; p != t; p=p->next)
			q = p;
		
		if (q != NULL)
			q->next = t->next;
		else
			head = t->next;
		free(t);
	}
	return head;
}

/*stop job*/
list stop(list head, int jobID, int *f) 
{	
	list p;
	
	*f = 0;
	p = locateJob(head, jobID);
	if (p != NULL)
	{
		*f = 1;
		if (p->pid != -1)
			kill(p->pid, SIGKILL);
		head = updateList(head, p);
	}
	return head;
}

/* Kills all running jobs */
list stopAll(list head)
{
	int f = 0;
	while (head != NULL)
		head = stop(head, head->jobID, &f);
	return head;
}
		
/*remove running job with a specific pid from list*/
list removeJobpid(list head, int pid)
{
	list p;
	
	p = locateJobpid(head, pid);
	head = updateList(head, p);
	return head;
}