#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<stdbool.h>

int nResource, mProcess;
int *resources;
int *totalRes;
int **allocated;
int **maxNeeded;
int **needLeft;
int *safeState;
int mProcessR = 0;

pthread_mutex_t lockR;
pthread_cond_t condition;
bool getSafeState();
void* processCode(void* );

int main()
{
	printf("\nEnter the number of Processes : ");
	scanf("%d", &mProcess);

	printf("Enter the number of Resources : ");
	scanf("%d", &nResource);

	totalRes = (int *)malloc(nResource * sizeof(*totalRes));
	printf("\nTotal resources : ");
	for(int i =0;i<nResource;i++)
	{
	scanf("%d", &totalRes[i]);
	}

	resources = (int *)malloc(nResource * sizeof(*resources));
	printf("\nCurrently available resources : ");
	for(int i =0;i<nResource;i++)
	{
	scanf("%d", &resources[i]);
	}

	allocated = (int **)malloc(mProcess * sizeof(*allocated));
        for(int i =0;i<mProcess;i++)
        {
	allocated[i] = (int *)malloc(nResource * sizeof(**allocated));
     	}

	maxNeeded = (int **)malloc(mProcess * sizeof(*maxNeeded));
        for(int i =0;i<mProcess;i++)
        {
        maxNeeded[i] = (int *)malloc(nResource * sizeof(**maxNeeded));
        }

	printf("\n");
	for(int i=0;i<mProcess;i++)
	{
	int j;
	printf("\nResources allocated to P%d : ", i);
	for(j =0;j<nResource; j++){
	scanf("%d", &allocated[i][j]);}
	}

	printf("\n");
        for(int i=0;i<mProcess;i++)
        {
	int j;
        printf("\nMaximum Resources needed by R%d  : ", i);
        for(j = 0;j<nResource; j++){
        scanf("%d", &maxNeeded[i][j]);}
        }
	printf("\n");

	needLeft = (int **)malloc(mProcess * sizeof(*needLeft));
	for(int i=0;i<mProcess;i++)
	{
	needLeft[i] = (int *)malloc(nResource * sizeof(**needLeft));
	}

	for(int i=0;i<mProcess;i++)
	{
	int j;
	for(j=0;j<nResource;j++)
	{
	needLeft[i][j] = maxNeeded[i][j] - allocated[i][j];
	}
	}

	safeState = (int *)malloc(mProcess * sizeof(*safeState));
	for(int i=0;i<mProcess;i++)
	{
	safeState[i] = -1;
	}
	if(!getSafeState())
	{
	printf("\n Unsafe state!");
	exit(-1);
	}

	printf("\n\nSafe Sequence : ");
	for(int i=0;i<mProcess;i++)
	{
	printf("%-3d", safeState[i]);
	}

	printf("\nExecuting Processes...\n\n");
	sleep(1);

	pthread_t processes[mProcess];
	pthread_attr_t attrP;
	pthread_attr_init(&attrP);
	
	int pNumber[mProcess];
	for(int i=0;i<mProcess; i++)
	{ pNumber[i] = i;}

	for(int i=0;i<mProcess; i++)
	{
	pthread_create(&processes[i],&attrP,processCode,(void *)(&pNumber[i]));
	//pthread_join(processes[i],NULL);
	}

	for(int i=0;i<mProcess; i++)
	{
	pthread_join(processes[i],NULL);
	}

	printf("\n All Processes finished.\n");
}

bool getSafeState()
{
	int Avail[nResource];
	for(int i=0;i<nResource;i++){
	Avail[i] = resources[i];}

	bool finished[mProcess];
	for(int i=0;i<mProcess;i++){
	finished[i] = false;}

	int nfinished=0;
	while(nfinished < mProcess)
	{
	bool safe = false;
	for(int i=0;i<mProcess;i++)
	{
	if(!finished[i])
	{
		int j;
		bool possible = true;
		for(j=0;j<nResource;j++)
		{
		if(needLeft[i][j]>Avail[j]){
		possible = false;
		break;}
		}

		if(possible)
		{
		for(j=0;j<nResource;j++)
		{
		Avail[j] += allocated[i][j];
		}
		safeState[nfinished] = i;
		finished[i] = true;
		++nfinished;
		safe = true;
		}
	}
	}
	if(!safe)
	{
	for(int p=0;p<mProcess;p++)
	{
	safeState[p] = -1;
	}
	return false;
	}
	}
	return true;
}

void* processCode(void *arg)
{
	int p = *((int *) arg);
	pthread_mutex_lock(&lockR);

	while(p != safeState[mProcessR])
	{
	pthread_cond_wait(&condition,&lockR);
	}

	printf("\nP%d Executing...", p);
	printf("\nAllocated : ");
	for(int i=0;i<nResource;i++)
	{
	printf("%3d", allocated[p][i]);
	}

	printf("\nNeeded    : ");
	for(int i=0;i<nResource;i++)
	{
	printf("%3d", needLeft[p][i]);
	}

	printf("\nAvailable : ");
	for(int i=0;i<nResource;i++)
	{
	printf("%3d", resources[i]);
	}

	printf("\n");
	sleep(3);

	for(int i=0;i<nResource;i++)
	{
	resources[i] +=allocated[p][i];
	}
	printf("\n\tNow available : ");
	for(int i=0;i<nResource;i++)
	{
	printf("%3d", resources[i]);
	}
	printf("\n\n");

	sleep(1);

	mProcessR++;
	pthread_cond_broadcast(&condition);
	pthread_mutex_unlock(&lockR);
	pthread_exit(NULL);
}	






