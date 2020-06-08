/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#include <stdio.h>
/*#include <stdlib.h>*/
#include <unistd.h>
#include <stdint.h>
#include <string.h>
/*#include <errno.h>*/
/*#include <signal.h>*/
/*#include <time.h>*/
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "scb.h"

int scb_create(char *name, uint16_t totalElements, size_t sizeElements, scb_t *ctx)
{
	int fdshmem = 0;
	size_t szshmem = 0;
	void *shmem = NULL;

	szshmem = sizeof(scb_t) + (totalElements * sizeElements);

	fdshmem = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(fdshmem == -1){
		return(SCB_ERR_CREATE);
	}

	if(ftruncate(fdshmem, szshmem) == -1){
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	/* MAP_HUGETLB */
	shmem = mmap(NULL, szshmem, PROT_READ | PROT_WRITE, MAP_SHARED, fdshmem, 0);
	if(shmem == MAP_FAILED){
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	ctx = (scb_t *)shmem;

	ctx->head = 0;
	ctx->tail = 0;

	ctx->dataTotal     = totalElements;
	ctx->dataElementSz = sizeElements;

	if(sem_init(&ctx->empty, 1, 1) == -1){
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	if(sem_init(&ctx->full, 1, 0) == -1){
		sem_destroy(&ctx->empty);
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	if(sem_init(&ctx->buffCtrl, 1, 1) == -1){
		sem_destroy(&ctx->empty);
		sem_destroy(&ctx->full);
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	ctx->data = (void *)(shmem + sizeof(scb_t));
	memset(ctx->data, 0, szshmem);

	return(SCB_OK);
}

int scb_attach(scb_t *ctx, char *name)
{
	int fdshmem = 0;
	void *shmem = NULL;

	fdshmem = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
	if(fdshmem == -1){
		return(SCB_ERR_CREATE);
	}

	shmem = mmap(NULL, sizeof(scb_t), PROT_READ | PROT_WRITE, MAP_SHARED, fdshmem, 0);
	if(shmem == MAP_FAILED){
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	ctx = (scb_t *)shmem;

	/* ... */

	return(SCB_OK);
}

int scb_get(scb_t *ctx)
{

	return(SCB_OK);
}

int scb_put(scb_t *ctx)
{

	return(SCB_OK);
}

int scb_iterator_create(scb_t *ctx)
{

	return(SCB_OK);
}

int scb_iterator_get(scb_t *ctx)
{

	return(SCB_OK);
}

int scb_destroy(scb_t *ctx)
{

	return(SCB_OK);
}
