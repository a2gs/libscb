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
#include <errno.h>
/*#include <signal.h>*/
/*#include <time.h>*/
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "scb.h"

scb_err_t scb_create(char *name, uint16_t totalElements, size_t sizeElements, scb_t *ctx, int *err)
{
	int fdshmem = 0;
	size_t szshmem = 0;
	void *shmem = NULL;

	strncpy(ctx->name, name, SCB_NAME_MAXSZ);

	szshmem = sizeof(scb_t) + (totalElements * sizeElements);

	fdshmem = shm_open(name, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
	if(fdshmem == -1){
		*err = errno;
		return(SCB_SHMEM);
	}

	if(ftruncate(fdshmem, szshmem) == -1){
		*err = errno;
		shm_unlink(name);
		return(SCB_FTRUNC);
	}

	/* MAP_HUGETLB */
	shmem = mmap(NULL, szshmem, PROT_READ | PROT_WRITE, MAP_SHARED, fdshmem, 0);
	if(shmem == MAP_FAILED){
		*err = errno;
		shm_unlink(name);
		return(SCB_MMAP);
	}

	ctx = (scb_t *)shmem;

	ctx->ctrl.head = 0;
	ctx->ctrl.tail = 0;
	ctx->ctrl.qtd  = 0;

	ctx->ctrl.dataTotal     = totalElements;
	ctx->ctrl.dataElementSz = sizeElements;

	if(sem_init(&(ctx->ctrl.empty), 1, 1) == -1){
		*err = errno;
		shm_unlink(name);
		return(SCB_SEMPH);
	}

	if(sem_init(&(ctx->ctrl.full), 1, 0) == -1){
		*err = errno;
		sem_destroy(&ctx->ctrl.empty);
		shm_unlink(name);
		return(SCB_SEMPH);
	}

	if(sem_init(&(ctx->ctrl.buffCtrl), 1, 1) == -1){
		*err = errno;
		sem_destroy(&ctx->ctrl.empty);
		sem_destroy(&ctx->ctrl.full);
		shm_unlink(name);
		return(SCB_SEMPH);
	}

	ctx->data = (void *)(shmem + sizeof(scb_t));

	*err = 0;
	return(SCB_OK);
}

scb_err_t scb_attach(scb_t *ctx, char *name, int *err)
{
	int fdshmem = 0;
	void *shmem = NULL;

	strncpy(ctx->name, name, SCB_NAME_MAXSZ);

	fdshmem = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
	if(fdshmem == -1){
		*err = errno;
		return(SCB_SHMEM);
	}

	shmem = mmap(NULL, sizeof(scb_t), PROT_READ | PROT_WRITE, MAP_SHARED, fdshmem, 0);
	if(shmem == MAP_FAILED){
		*err = errno;
		shm_unlink(name);
		return(SCB_SHMEM);
	}

	ctx = (scb_t *)shmem;
	ctx->data = (void *)(shmem + sizeof(scb_t));

	*err = 0;
	return(SCB_OK);
}

/* copyElement(..., ..., size_t n) n is not used, instead ctx->ctrl.dataElementSz */
scb_err_t scb_get(scb_t *ctx, void *element,  void *(*copyElement)(void *dest, const void *src, size_t n))
{
	sem_wait(&(ctx->ctrl.full));
	sem_wait(&(ctx->ctrl.buffCtrl));

	copyElement(element, ctx->data + (ctx->ctrl.tail * ctx->ctrl.dataElementSz), ctx->ctrl.dataElementSz);

	ctx->ctrl.tail = (ctx->ctrl.tail + 1) % ctx->ctrl.dataElementSz;
	ctx->ctrl.qtd--;

	sem_post(&(ctx->ctrl.buffCtrl));
	sem_post(&(ctx->ctrl.empty));

	return(SCB_OK);
}

/* copyElement(..., ..., size_t n) n is not used, instead ctx->ctrl.dataElementSz */
scb_err_t scb_put(scb_t *ctx, void *element, void *(*copyElement)(void *dest, const void *src, size_t n))
{
	sem_wait(&(ctx->ctrl.empty));
	sem_wait(&(ctx->ctrl.buffCtrl));

	copyElement(ctx->data + (ctx->ctrl.head * ctx->ctrl.dataElementSz), element, ctx->ctrl.dataElementSz);

	ctx->ctrl.head = (ctx->ctrl.head + 1) % ctx->ctrl.dataElementSz;
	ctx->ctrl.qtd++;

	sem_post(&(ctx->ctrl.buffCtrl));
	sem_post(&(ctx->ctrl.full));

	return(SCB_OK);
}

scb_err_t scb_iterator_create(scb_t *ctx, scb_iter_t *ctxIter)
{
	ctxIter->it = ctx->ctrl.tail;

	return(SCB_OK);
}

scb_err_t scb_iterator_get(scb_t *ctx, scb_iter_t *ctxIter, void *data)
{
	if((ctx->ctrl.head < ctxIter->it) || (ctx->ctrl.tail > ctxIter->it)){
		ctxIter->it = ctx->ctrl.tail;
	}

	return(SCB_OK);
}

scb_err_t scb_destroy(scb_t *ctx, int *err)
{
	int ret = 0;

	ret = sem_destroy(&(ctx->ctrl.buffCtrl)) | sem_destroy(&(ctx->ctrl.empty)) | sem_destroy(&(ctx->ctrl.full));

	if(ret != 0){
		*err = errno;
		return(SCB_SEMPH);
	}

	if(shm_unlink(ctx->name) == -1){
		*err = errno;
		return(SCB_SHMEM);
	}

	return(SCB_OK);
}

void scb_strerror(scb_err_t err, int ret, char *msg)
{
	char *errat = NULL;

	switch(err){
		case SCB_SHMEM:
			errat = "Shared Memory";
			break;
		case SCB_FTRUNC:
			errat = "FTruncate";
			break;
		case SCB_SEMPH:
			errat = "Semaphore";
			break;
		case SCB_MMAP:
			errat = "mmap";
			break;
		default:
			errat = "Success";
			break;
	}

	snprintf(msg, SCB_ERRORMSG_MAXSZ, "Error at [%s]: [%s]\n", errat, strerror(ret));
}
