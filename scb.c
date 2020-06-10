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

	strncpy(ctx->name, name, SCB_NAME_MAXSZ);

	szshmem = sizeof(scb_ctrl_t) + (totalElements * sizeElements);

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

	ctx->ctrl->head = 0;
	ctx->ctrl->tail = 0;

	ctx->ctrl->dataTotal     = totalElements;
	ctx->ctrl->dataElementSz = sizeElements;

	if(sem_init(&ctx->ctrl->empty, 1, 1) == -1){
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	if(sem_init(&ctx->ctrl->full, 1, 0) == -1){
		sem_destroy(&ctx->ctrl->empty);
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	if(sem_init(&ctx->ctrl->buffCtrl, 1, 1) == -1){
		sem_destroy(&ctx->ctrl->empty);
		sem_destroy(&ctx->ctrl->full);
		shm_unlink(name);
		return(SCB_ERR_CREATE);
	}

	ctx->data = (void *)(shmem + sizeof(scb_ctrl_t));

	return(SCB_OK);
}

int scb_attach(scb_t *ctx, char *name)
{
	int fdshmem = 0;
	void *shmem = NULL;

	strncpy(ctx->name, name, SCB_NAME_MAXSZ);

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
	ctx->data = (void *)(shmem + sizeof(scb_ctrl_t));

	return(SCB_OK);
}

int scb_get(scb_t *ctx)
{
	sem_wait(&(ctx->ctrl->full));
	sem_wait(&(ctx->ctrl->buffCtrl));

	/* ... get an element from circ buffer ... */

	sem_post(&(ctx->ctrl->buffCtrl));
	sem_post(&(ctx->ctrl->empty));

	return(SCB_OK);
}

int scb_put(scb_t *ctx)
{
	sem_wait(&(ctx->ctrl->empty));
	sem_wait(&(ctx->ctrl->buffCtrl));

	/* ... put an element to circ buffer ... */

	sem_post(&(ctx->ctrl->buffCtrl));
	sem_post(&(ctx->ctrl->full));

	return(SCB_OK);
}

int scb_iterator_create(scb_t *ctx, scb_iter_t *ctxIter)
{
	ctxIter->it = ctx->ctrl->tail;

	return(SCB_OK);
}

int scb_iterator_get(scb_t *ctx, scb_iter_t *ctxIter, void *data)
{
	if((ctx->ctrl->head < ctxIter->it) || (ctx->ctrl->tail > ctxIter->it)){
		ctxIter-> it = ctx->ctrl->tail;
	}

	return(SCB_OK);
}

int scb_destroy(scb_t *ctx)
{
	sem_destroy(&(ctx->ctrl->buffCtrl));
	sem_destroy(&(ctx->ctrl->empty));
	sem_destroy(&(ctx->ctrl->full));
	shm_unlink(ctx->name);

	return(SCB_OK);
}
