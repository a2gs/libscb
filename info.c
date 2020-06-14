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
	scb_t inf;
	scb_err_t scberr;

	if(argc != 2){
		printf("Usage: %s [SEMAPHORE_NAME]\n", argv[0]);
		return(1);
	}

	scberr = scb_getInfo(argv[1], &inf, &err);
	if(scberr != SCB_OK){
		scb_strerror(scberr, err, scberrormsg);
		printf("%s", scberrormsg);
		return(1);
	}

	printf("Circular buffer name: [%s]\n", inf.name);
	printf("Head................: [%u]\n", inf.ctrl.head);
	printf("Tail................: [%u]\n", inf.ctrl.tail);
	printf("Qtd.................: [%u]\n", inf.ctrl.qtd);
	printf("Total capacity......: [%u]\n", inf.ctrl.dataTotal);
	printf("Element size (bytes): [%lu]\n", inf.ctrl.dataElementSz);

	return(0);
}
