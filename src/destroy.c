/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#include <stdio.h>

#include "scb.h"
#include "prodcons.h"

int main(int argc, char *argv[])
{
	int err = 0;
	scb_err_t scberr;

	if(argc != 2){
		printf("Usage: %s [SEMAPHORE_NAME]\n", argv[0]);
		return(1);
	}

	scberr = scb_destroy(argv[1], &err);
	SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, err, 1);

	return(0);
}
