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

int main(int argc, char *argv[])
{
	int err = 0;
	char scberrormsg[SCB_ERRORMSG_MAXSZ + 1] = {'\0'};
	scb_err_t scberr;

	if(argc != 2){
		printf("Usage: %s [SEMAPHORE_NAME]\n", argv[0]);
		return(1);
	}

	scberr = scb_destroy(argv[1], &err);

	if(scberr != SCB_OK){
		scb_strerror(scberr, err, scberrormsg);
		printf("%s", scberrormsg);
		return(1);
	}

	return(0);
}
