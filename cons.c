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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "scb.h"
#include "prodcons.h"

int main(int argc, char *argv[])
{
	int ret = 0;
	unsigned int sec = 0;
	scb_t ctx;
	scb_err_t scberr;
	element_t e;
	pid_t p;

	if(argc != 3){
		printf("Usage: %s [SEMAPHORE_NAME] [SECONDS]\n", argv[0]);
		return(1);
	}

	sec = atoi(argv[2]);
	p = getpid();

	scberr = scb_attach(&ctx, argv[1], &ret);
	SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, ret, 1);

	while(1){
		memset(&e, 0, sizeof(element_t));

		scberr = scb_get(&ctx, &e, copyElement, SCB_BLOCK, &ret);
		SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, ret, 1);

		printf("PID [%d] got [%d][%f][%s]\n", p, e.a, e.b, e.c);

		sleep(sec);
	}

	return(0);
}
