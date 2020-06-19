/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#ifndef __SCB_PRODCONS_SAMPLE__
#define __SCB_PRODCONS_SAMPLE__

typedef struct _element_t{
	int a;
	float b;
	char c[10];
}element_t;

#define SCB_SAMPLE_CHECK_ERROR(__SCBSAMPLE_RET, __SCB_SAMPLE_APIERR, __SCB_SAMPLE_ERRNO, __SCB_SAMPLE_RET) \
{                                                                                                          \
	if(__SCBSAMPLE_RET != __SCB_SAMPLE_APIERR){                                                             \
		char scberrormsg[SCB_ERRORMSG_MAXSZ + 1] = {'\0'};                                                   \
		scb_strerror(__SCB_SAMPLE_APIERR, __SCB_SAMPLE_ERRNO, scberrormsg);                                  \
		printf("%s", scberrormsg);                                                                           \
		return(__SCB_SAMPLE_RET);                                                                            \
	}                                                                                                       \
}

#include <string.h>
void * copyElement(void *dst, const void *src)
{
	return(memcpy(dst, src, sizeof(element_t)));
}

#endif
