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

#include "scb.h"
#include "prodcons.h"

int main(int argc, char *argv[])
{
	int ret = 0;
	unsigned int sec = 0;
	scb_t ctx;

	if(argc != 3){
		printf("Usage: %s [SEMAPHORE_NAME] [SECONDS]", argv[0]);
		return(1);
	}

	sec = atoi(argv[2]);

	if(scb_create(argv[1], 100, sizeof(element_t), &ctx, &ret) != SCB_OK){
		return(1);
	}

	while(1){
/*		scb_put(&ctx); */
		sleep(sec);
	}

	scb_destroy(&ctx, &ret);

	return(0);
}
