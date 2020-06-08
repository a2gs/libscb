/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#include "scb.h"
#include "prodcons.h"

int main(int argc, char *argv[])
{
	char name[] = "./sem1";
	scb_t ctx;

	if(scb_attach(&ctx, name) != SCB_OK){
	}


	return(0);
}
