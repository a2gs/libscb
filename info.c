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
	int semFull = 0;
	int semEmpty = 0;
	int semBuff = 0;
	scb_ctrl_t inf;
	scb_err_t scberr;

	if(argc != 2){
		printf("Usage: %s [SEMAPHORE_NAME]\n", argv[0]);
		return(1);
	}

	scberr = scb_getInfo(argv[1], &inf, &semFull, &semEmpty, &semBuff, &err);
	SCB_SAMPLE_CHECK_ERROR(SCB_OK, scberr, err, 1);

	printf("Circular buffer name: [%s]\n", argv[1]);
	printf("Head................: [%u]\n", inf.head);
	printf("Tail................: [%u]\n", inf.tail);
	printf("Qtd.................: [%u]\n", inf.qtd);
	printf("Total capacity......: [%u]\n", inf.dataTotal);
	printf("Element size (bytes): [%lu]\n", inf.dataElementSz);
	printf("Semaphores..........: Full [%d] | Empty [%d] | Buffer control [%d]\n", semFull, semEmpty, semBuff);

	return(0);
}
