/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
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

	szshmem = sizeof(scb_ctrl_t) + (totalElements * sizeElements);

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

	close(fdshmem);

	ctx->ctrl = (scb_ctrl_t *)shmem;

	strncpy(ctx->name, name, SCB_NAME_MAXSZ);

	ctx->ctrl->head = 0;
	ctx->ctrl->tail = 0;
	ctx->ctrl->qtd  = 0;

	ctx->ctrl->dataTotal     = totalElements;
	ctx->ctrl->dataElementSz = sizeElements;

	ctx->data = (void *)(shmem + sizeof(scb_ctrl_t));

	if(sem_init(&(ctx->ctrl->empty), 1, totalElements) == -1){
		*err = errno;
		shm_unlink(name);
		return(SCB_SEMPH);
	}

	if(sem_init(&(ctx->ctrl->full), 1, 0) == -1){
		*err = errno;
		sem_destroy(&ctx->ctrl->empty);
		shm_unlink(name);
		return(SCB_SEMPH);
	}

	if(sem_init(&(ctx->ctrl->buffCtrl), 1, 1) == -1){
		*err = errno;
		sem_destroy(&ctx->ctrl->empty);
		sem_destroy(&ctx->ctrl->full);
		shm_unlink(name);
		return(SCB_SEMPH);
	}

	*err = 0;
	return(SCB_OK);
}

scb_err_t scb_attach(scb_t *ctx, char *name, int *err)
{
	int fdshmem = 0;
	int sf = 0, se = 0, sb = 0;
	size_t szshmem = 0;
	void *shmem = NULL;
	scb_ctrl_t scbInf;
	scb_err_t scberr;

	scberr = scb_getInfo(name, &scbInf, &sf, &se, &sb, err);
	if(scberr != SCB_OK) return(scberr);

	szshmem = sizeof(scb_ctrl_t) + (scbInf.dataTotal * scbInf.dataElementSz);

	fdshmem = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR);
	if(fdshmem == -1){
		*err = errno;
		return(SCB_SHMEM);
	}

	shmem = mmap(NULL, szshmem, PROT_READ | PROT_WRITE, MAP_SHARED, fdshmem, 0);
	if(shmem == MAP_FAILED){
		*err = errno;
		shm_unlink(name);
		return(SCB_SHMEM);
	}

	close(fdshmem);

	ctx->ctrl = (scb_ctrl_t *)shmem;
	ctx->data = (void *)(shmem + sizeof(scb_ctrl_t));
	strncpy(ctx->name, name, SCB_NAME_MAXSZ);

	*err = 0;
	return(SCB_OK);
}

scb_err_t scb_get(scb_t *ctx, void *element,  void *(*copyElement)(void *dest, const void *src), scb_block_t block, int *err)
{
	scb_err_t ret = SCB_OK;

	if(block == SCB_UNBLOCK){
		if(sem_trywait(&(ctx->ctrl->full)) == -1)
			return(SCB_BLOCKED);
	}else{
		if(sem_wait(&(ctx->ctrl->full)) == -1){
			*err = errno;
			return(SCB_SEMPH);
		}
	}

	if(sem_wait(&(ctx->ctrl->buffCtrl)) == -1){
		*err = errno;
		sem_post(&(ctx->ctrl->full));
		return(SCB_SEMPH);
	}

	if(ctx->ctrl->qtd == 0)
		ret = SCB_EMPTY;
	else{
		copyElement(element, ctx->data + (ctx->ctrl->tail * ctx->ctrl->dataElementSz));

		ctx->ctrl->tail = (ctx->ctrl->tail + 1) % ctx->ctrl->dataTotal;
		ctx->ctrl->qtd--;
	}

	sem_post(&(ctx->ctrl->buffCtrl));
	sem_post(&(ctx->ctrl->empty));

	*err = 0;
	return(ret);
}

scb_err_t scb_put(scb_t *ctx, void *element, void *(*copyElement)(void *dest, const void *src), scb_block_t block, int *err)
{
	scb_err_t ret = SCB_OK;

	if(block == SCB_UNBLOCK){
		if(sem_trywait(&(ctx->ctrl->empty)) == -1)
			return(SCB_BLOCKED);
	}else{
		if(sem_wait(&(ctx->ctrl->empty)) == -1){
			*err = errno;
			return(SCB_SEMPH);
		}
	}

	if(sem_wait(&(ctx->ctrl->buffCtrl)) == -1){
		*err = errno;
		sem_post(&(ctx->ctrl->full));
		return(SCB_SEMPH);
	}

	if(ctx->ctrl->qtd == ctx->ctrl->dataTotal)
		ret = SCB_FULL;
	else{
		copyElement(ctx->data + (ctx->ctrl->head * ctx->ctrl->dataElementSz), element);

		ctx->ctrl->head = (ctx->ctrl->head + 1) % ctx->ctrl->dataTotal;
		ctx->ctrl->qtd++;
	}

	sem_post(&(ctx->ctrl->buffCtrl));
	sem_post(&(ctx->ctrl->full));

	*err = 0;
	return(ret);
}

scb_err_t scb_iterator_create(scb_t *ctx, scb_iter_t *ctxIter)
{
	ctxIter->it = ctx->ctrl->tail;
	ctxIter->qtd = ctx->ctrl->qtd;

	return(SCB_OK);
}

scb_err_t scb_iterator_get(scb_t *ctx, scb_iter_t *ctxIter, void *data, void *(*copyElement)(void *dest, const void *src))
{
	/* Iterator doesnt lock de scb */

	if(ctxIter->qtd == 0)
		return(SCB_ITER_END);

	copyElement(data, ctx->data + (ctxIter->it * ctx->ctrl->dataElementSz));

	ctxIter->it = (ctxIter->it + 1) % ctx->ctrl->dataTotal;
	ctxIter->qtd--;

	return(SCB_OK);
}

scb_err_t scb_lock(scb_t *ctx, scb_block_t block, int *err)
{
	/* Ugly ... but safe (and fast)! */

	if(block == SCB_BLOCK){

		if(sem_wait(&(ctx->ctrl->full)) == -1){
			*err = errno;
			return(SCB_SEMPH);
		}

		if(sem_wait(&(ctx->ctrl->empty)) == -1){
			*err = errno;
			sem_post(&(ctx->ctrl->full));
			return(SCB_SEMPH);
		}

		if(sem_wait(&(ctx->ctrl->buffCtrl)) == -1){
			*err = errno;
			sem_post(&(ctx->ctrl->full));
			sem_post(&(ctx->ctrl->empty));
			return(SCB_SEMPH);
		}

	}else{

		if(sem_post(&(ctx->ctrl->empty)) == -1){
			*err = errno;
			return(SCB_SEMPH);
		}

		if(sem_post(&(ctx->ctrl->full)) == -1){
			*err = errno;
			sem_wait(&(ctx->ctrl->empty));
			return(SCB_SEMPH);
		}

		if(sem_post(&(ctx->ctrl->buffCtrl)) == -1){
			*err = errno;
			sem_wait(&(ctx->ctrl->full));
			sem_wait(&(ctx->ctrl->empty));
			return(SCB_SEMPH);
		}
	}

	*err = 0;
	return(SCB_OK);
}

scb_err_t scb_destroy(char *name, int *err)
{
	int ret = 0;
	scb_t ctx;
	scb_err_t scberr;

	scberr = scb_attach(&ctx, name, err);
	if(scberr != SCB_OK) return(scberr);

	ret = sem_destroy(&(ctx.ctrl->buffCtrl)) | sem_destroy(&(ctx.ctrl->empty)) | sem_destroy(&(ctx.ctrl->full));

	if(ret != 0){
		*err = errno;
		return(SCB_SEMPH);
	}

	if(shm_unlink(ctx.name) == -1){
		*err = errno;
		return(SCB_SHMEM);
	}

	return(SCB_OK);
}

void scb_strerror(scb_err_t err, int ret, char *msg)
{
	char *errat = NULL;
	char errdesc[SCB_ERRORMSG_MAXSZ] = {'\0'};

	switch(err){
		case SCB_SHMEM:
			errat = "Shared Memory";
			/* TODO strerror_r(ret, errdesc, SCB_ERRORMSG_MAXSZ); */
			strncpy(errdesc, strerror(ret), SCB_ERRORMSG_MAXSZ);
			break;
		case SCB_FTRUNC:
			errat = "FTruncate";
			/* TODO strerror_r(ret, errdesc, SCB_ERRORMSG_MAXSZ); */
			strncpy(errdesc, strerror(ret), SCB_ERRORMSG_MAXSZ);
			break;
		case SCB_SEMPH:
			errat = "Semaphore";
			/* TODO strerror_r(ret, errdesc, SCB_ERRORMSG_MAXSZ); */
			strncpy(errdesc, strerror(ret), SCB_ERRORMSG_MAXSZ);
			break;
		case SCB_MMAP:
			errat = "mmap";
			/* TODO strerror_r(ret, errdesc, SCB_ERRORMSG_MAXSZ); */
			strncpy(errdesc, strerror(ret), SCB_ERRORMSG_MAXSZ);
			break;
		case SCB_FULL:
			errat = "SCB FULL";
			strncpy(errdesc, "There is no space", SCB_ERRORMSG_MAXSZ);
			break;
		case SCB_EMPTY:
			errat = "SCB EMPTY";
			strncpy(errdesc, "There are no elements", SCB_ERRORMSG_MAXSZ);
			break;
		case SCB_BLOCKED:
			errat = "SCB BLOCKED";
			strncpy(errdesc, "Access blocked (in use at operation side or full or empty. Try again)", SCB_ERRORMSG_MAXSZ);
			break;
		default:
			errat = "Success";
			strncpy(errdesc, "no error", SCB_ERRORMSG_MAXSZ);
			break;
	}

	snprintf(msg, SCB_ERRORMSG_MAXSZ, "Error at [%s]: [%s]\n", errat, errdesc);
}

scb_err_t scb_getInfo(char *name, scb_ctrl_t *inf, int *sf, int *se, int *sb, int *err)
{
	int fdshmem = 0;
	void *shmem = NULL;

	fdshmem = shm_open(name, O_RDONLY, S_IRUSR | S_IWUSR);
	if(fdshmem == -1){
		*err = errno;
		return(SCB_SHMEM);
	}

	shmem = mmap(NULL, sizeof(scb_t), PROT_READ, MAP_SHARED, fdshmem, 0);
	if(shmem == MAP_FAILED){
		*err = errno;
		shm_unlink(name);
		return(SCB_SHMEM);
	}

	memcpy(inf, shmem, sizeof(scb_ctrl_t));

	sem_getvalue(&((scb_ctrl_t *) shmem)->full, sf);
	sem_getvalue(&((scb_ctrl_t *) shmem)->empty, se);
	sem_getvalue(&((scb_ctrl_t *) shmem)->buffCtrl, sb);

	close(fdshmem);

	return(SCB_OK);
}
