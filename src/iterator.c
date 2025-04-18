/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

#include "scb.h"
#include "prodcons.h"

int main(int argc, char *argv[])
{
	int err = 0;
	scb_t ctx;
	scb_iter_t it;
	scb_err_t scberr;
	element_t e;
	pid_t p;

	if(argc != 2){
		printf("Usage: %s [SEMAPHORE_NAME]\n", argv[0]);
		return(1);
	}

	p = getpid();

	scberr = scb_attach(&ctx, argv[1], &err);
	SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, err, 1);

	printf("Locking the scb from another processes.\n");

	scberr = scb_lock(&ctx, SCB_BLOCK, &err);
	SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, err, 1);

	/* Iterator is a tail and head position snapshot, but data
	 * may changes between fetches (by another processes).
	 * That's cuz we call scb_lock() before and unlock after.
	 */
	scberr = scb_iterator_create(&ctx, &it);
	SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, err, 1);

	while(1){
		scberr = scb_iterator_get(&ctx, &it, &e, copyElement);
		if(scberr == SCB_ITER_END) break;

		SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, err, 1);

		printf("PID [%d] got [%d][%f][%s]\n", p, e.a, e.b, e.c);
	}

	printf("Unlocking the scb from another processes.\n");

	scberr = scb_lock(&ctx, SCB_UNBLOCK, &err);
	SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, err, 1);

	return(0);
}
