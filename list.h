typedef struct node *list;

struct node
{
	char job[100];
	int jobID;
	int pid;
	list next;
};

extern list rl;
extern list ql;

int count(list);
list newJob(int, char *, int);
list insertJob(list, int, char *, int);
void print(list, char **);
list locateJob(list, int);
list updateList(list, list);
list stop(list, int, int *);
list removeJobpid(list, int);
list stopAll(list);