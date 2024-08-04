/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "scb.h" 

#define TOTAL_ELEMENTS	(50)
#define SIZE_ELEMENT		(50)

unsigned int TOTAL_MSGS = 0;
unsigned int TOTAL_THREADS = 0;
unsigned int DELAY = 0;
char MODE = 0;
char RND_START_DELAY = 0;
char *NAMED_QUEUE = NULL;

void sig(int sigNum)
{
	switch(sigNum){
		case SIGINT:
			printf("Killing all children.\n");
			killpg(getpid(), SIGTERM);
			break;

		case SIGUSR2:
		case SIGTERM:
			break;
	}

	return;
}

typedef struct
{
	pid_t selfPID;
	unsigned int idProc;
	unsigned int idThread;
}threadInfo_t;

void * copyElement(void *dst, const void *src)
{
	return(memcpy(dst, src, SIZE_ELEMENT));
}

void * runProducerThread(void *_info)
{
	int ret = 0;
	unsigned int i = 0;
	char msg[SIZE_ELEMENT + 1] = {0};
	threadInfo_t *info = _info;
	scb_t ctx = {0};
	scb_err_t scberr = SCB_OK;
	struct timeval tv;
	struct tm ptm;

	// Creating or attaching SCB queue
	scberr = scb_create(NAMED_QUEUE, TOTAL_ELEMENTS, SIZE_ELEMENT, &ctx, &ret);
	if(scberr == SCB_EEXIST){
		scberr = scb_attach(&ctx, NAMED_QUEUE, &ret);

		if(scberr != SCB_OK){
			// TODO : report erro
			pthread_exit(NULL);
		}
	}else if(scberr != SCB_OK){
		// TODO : report erro
		pthread_exit(NULL);
	}

	// TODO: rand start here

	// Populating
	for(i = 0; ; ){
		if(TOTAL_MSGS != 0){
			if(i == TOTAL_MSGS) break;
			i++;
		}

		gettimeofday(&tv, NULL);
		localtime_r(&tv.tv_sec, &ptm);

		snprintf(msg, SIZE_ELEMENT, "[%d %u %u]-[%02d/%02d/%04d %02d:%02d:%02d.%06ld]",
		         info->selfPID, info->idProc, info->idThread, ptm.tm_mday, ptm.tm_mon + 1,
		         ptm.tm_year + 1900, ptm.tm_hour, ptm.tm_min, ptm.tm_sec, tv.tv_usec);

		scberr = scb_put(&ctx, msg, copyElement, SCB_UNBLOCK, &ret);
		if(scberr != SCB_OK){
			// TODO : report erro
			pthread_exit(NULL);
		}

		usleep(DELAY);
	}

	pthread_exit(NULL);
}

void * runConsumerThread(void *_info)
{
	pthread_exit(NULL);
}

int runFork(unsigned int forkId, pid_t selfPID)
{
	unsigned int i = 0;
	int ptRet = 0;
	pthread_t *threads = NULL;
	threadInfo_t *threadsInfo = NULL;

	threads = malloc(sizeof(pthread_t) * TOTAL_THREADS);
	if(threads == NULL){
		return(1);
	}

	threadsInfo = malloc(sizeof(threadInfo_t) * TOTAL_THREADS);
	if(threadsInfo == NULL){
		return(1);
	}

	for(i = 0; i < TOTAL_THREADS; i++){
		threadsInfo[i].selfPID = selfPID;
		threadsInfo[i].idProc = forkId;
		threadsInfo[i].idThread = i;

		if(MODE == 'P') ptRet = pthread_create(&threads[i], NULL, runProducerThread, (void *) &threadsInfo[i]);
		else            ptRet = pthread_create(&threads[i], NULL, runConsumerThread, (void *) &threadsInfo[i]);
		if(ptRet != 0){
			printf("Create thread error: [%d] - [%s]\n", ptRet, strerror(ptRet));
			return(1);
		}
	}

	for(i = 0; i < TOTAL_THREADS; i++){
		pthread_join(threads[i], NULL);
	}

	free(threadsInfo);
	free(threads);

	return(0);
}

int main(int argc, char *argv[])
{
	unsigned int nforks = 0;
	unsigned int i = 0;
	pid_t p = 0;
	pid_t pChild = 0;

	if(argc != 8){
		printf("Usage:\n\t%s <MODE P|C> <QUEUE_NAME> <NUM_FORKS> <NUM_THREADS> <NUM_MSGS> <DELAY_BETWEEN_MSGS> <RND_START 0|1>\n\n", argv[0]);
		printf("Where:\n\tP|C - Producer or consumer mode\n");
		printf("\tMODE - Producer or consumer\n");
		printf("\tQUEUE_NAME - Path to named queue\n");
		printf("\tNUM_FORKS\n");
		printf("\tNUM_THREADS\n");
		printf("\tNUM_MSGS - Qtd msgs sent/received (0 = inf. ^c to stop)\n");
		printf("\tDELAY_BETWEEN_MSGS - millisec (0 = none)\n");
		printf("\tRND_START - rand millisec start\n");
		return(1);
	}

	signal(SIGINT, sig);

	MODE = argv[1][0];
	NAMED_QUEUE = argv[2];
	nforks = atoi(argv[3]);
	TOTAL_THREADS = atoi(argv[4]);
	TOTAL_MSGS = atoi(argv[5]);
	DELAY = atoi(argv[6]);
	RND_START_DELAY = atoi(argv[7]);

	if(((MODE != 'P') && (MODE != 'C')) ||
	   (nforks == 0) || (TOTAL_THREADS == 0) ||
	   ((RND_START_DELAY != 0) && (RND_START_DELAY != 0))){
		printf("Parameter value erro.\n");
		return(1);
	}

	for(i = 0; i < nforks; i++){
		p = fork();

		if(p == 0){
			signal(SIGINT, SIG_DFL);

			pChild = getpid();
			printf("Child id [%d] forked PID [%d]\n", i, pChild);

			runFork(i, pChild);

			return(0);

		}else if(p > 0){
			printf("Spawned fork child id [%d] PID [%d]\n", i, p);

		}else{
			printf("Error forking child id [%u]: [%s]\n", i, strerror(errno));
			return(1);

		}
	}

	while((p = wait(NULL)) > 0){
		printf("Child PID [%d] ends.\n", p);
	}

	printf("END!\n");

	return(0);
}
