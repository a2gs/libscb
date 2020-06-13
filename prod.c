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

#include "scb.h"
#include "prodcons.h"

void prodElement(element_t *e)
{
	static unsigned int i = 0;
	element_t m[] = {
		{0, 0.0, "000000000\0"}, {1, 1.1, "111111111\0"},
		{2, 2.2, "222222222\0"}, {3, 3.3, "333333333\0"},
		{4, 4.4, "444444444\0"}, {5, 5.5, "555555555\0"},
		{6, 6.6, "666666666\0"}, {7, 7.7, "777777777\0"},
		{8, 8.8, "888888888\0"}, {9, 9.9, "999999999\0"},
	};

	memcpy(e, &m[i], sizeof(element_t));
	i++;

	return;
}

int main(int argc, char *argv[])
{
	int ret = 0;
	unsigned int sec = 0;
	char scberrormsg[SCB_ERRORMSG_MAXSZ + 1] = {'\0'};
	scb_t *ctx;
	scb_err_t scberr = SCB_OK;
	element_t e;

	if(argc != 3){
		printf("Usage: %s [SEMAPHORE_NAME] [SECONDS]\n", argv[0]);
		return(1);
	}

	sec = atoi(argv[2]);

printf("11111\n");
	scberr = scb_create(argv[1], 100, sizeof(element_t), &ctx, &ret);
	if(scberr != SCB_OK){
		scb_strerror(scberr, ret, scberrormsg);
		printf("%s", scberrormsg);
		return(1);
	}

printf("11122\n");
	while(1){
		prodElement(&e);

printf("22222\n");
		scberr = scb_put(ctx, &e, copyElement);
		if(scberr != SCB_OK){
			scb_strerror(scberr, ret, scberrormsg);
			printf("%s", scberrormsg);
			return(1);
		}

		sleep(sec);
	}

printf("33333\n");
	scberr = scb_destroy(ctx, &ret);
	if(scberr != SCB_OK){
		scb_strerror(scberr, ret, scberrormsg);
		printf("%s", scberrormsg);
		return(1);
	}

	return(0);
}
