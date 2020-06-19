/* Andre Augusto Giannotti Scota (a2gs)
 * andre.scota@gmail.com
 * https://sites.google.com/view/a2gs
 *
 * SCB - Shared Circular Buffer
 *
 * MIT License
 */

#ifndef __LIB_SCB__
#define __LIB_SCB__

#include <stdint.h>
#include <semaphore.h>

#define SCB_NAME_MAXSZ (30)
#define SCB_ERRORMSG_MAXSZ (100)

typedef struct _scb_ctrl_t{
	sem_t empty;
	sem_t full;
	sem_t buffCtrl;

	uint16_t head;
	uint16_t tail;
	uint16_t qtd;

	uint16_t dataTotal; /* Total of elements (capacity) */
	size_t dataElementSz; /* Size of one element */
}scb_ctrl_t;

typedef struct _scb_t{
	char name[SCB_NAME_MAXSZ];
	scb_ctrl_t *ctrl;
	void *data;
}scb_t;

typedef struct _scb_iter_t{
	uint16_t it;
	uint16_t qtd;
}scb_iter_t;

typedef enum{
	SCB_OK = 0,
	SCB_SHMEM,
	SCB_FTRUNC,
	SCB_SEMPH,
	SCB_MMAP,
	SCB_FULL,
	SCB_EMPTY,
	SCB_BLOCKED,
	SCB_ITER_END
}scb_err_t;

typedef enum{
	SCB_BLOCK = 0,
	SCB_UNBLOCK
}scb_block_t;

/* scb_err_t scb_create(char *name, uint16_t totalElements, size_t sizeElements, scb_t *ctx, int *err)
 *
 * Parameters:
 *  name - In - 
 *  totalElements - In - 
 *  sizeElements - In - 
 *  ctx - Out - 
 *  err - Out - errno
 *
 * Return:
 *  SBC_OK - Success
 *  SCB_SHMEM - 
 *  SCB_FTRUNC - 
 *  SCB_SEMPH - 
 *  SCB_MMAP - 
 */
scb_err_t scb_create(char *name, uint16_t totalElements, size_t sizeElements, scb_t *ctx, int *err);

/* scb_err_t scb_attach(scb_t *ctx, char *name, int *err)
 *
 * Parameters:
 *  ctx - In - 
 *  name - In - 
 *  err - Out - errno
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_attach(scb_t *ctx, char *name, int *err);

/* scb_err_t scb_get(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src), scb_block_t block, int *ret)
 *
 * Parameters:
 *  ctx - In -
 *  element - Out -
 *  copyElement - In - 
 *  block - In -
 *  ret - Out - errno
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_get(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src), scb_block_t block, int *ret);

/* scb_err_t scb_put(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src), scb_block_t block, int *ret)
 *
 * Parameters:
 *  ctx - In -
 *  element - In - 
 *  copyElement - In - 
 *  block - In - 
 *  ret - Out - errno
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_put(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src), scb_block_t block, int *ret);

/* scb_err_t scb_getInfo(char *name, scb_ctrl_t *inf, int *sf, int *se, int *sb, int *err)
 *
 * Parameters:
 *  name - In -
 *  inf - Out -
 *  sf - Out - Full semaphore value
 *  se - Out - Empty semaphore value
 *  sb - Out - Buffer control semaphore value
 *  err - Out - errno
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_getInfo(char *name, scb_ctrl_t *inf, int *sf, int *se, int *sb, int *err);

/* scb_err_t scb_lock(scb_t *ctx, scb_block_t block, int *err)
 *
 * Parameters:
 *  ctx - In -
 *  block - in -
 *  err - Out - errno
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_lock(scb_t *ctx, scb_block_t block, int *err);

/* scb_err_t scb_iterator_create(scb_t *ctx, scb_iter_t *ctxIter)
 *
 * Parameters:
 *  ctx - In -
 *  ctxIter - Out -
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_iterator_create(scb_t *ctx, scb_iter_t *ctxIter);

/* scb_err_t scb_iterator_get(scb_t *ctx, scb_iter_t *ctxIter, void *data, void *(*copyElement)(void *dest, const void *src))
 *
 * Parameters:
 *  ctx - In -
 *  ctxIter - In -
 *  data - Out -
 *  copyElement - In -
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_iterator_get(scb_t *ctx, scb_iter_t *ctxIter, void *data, void *(*copyElement)(void *dest, const void *src));

/* scb_err_t scb_destroy(char *name, int *err)
 *
 * Parameters:
 *  name - In -
 *  err - Out - errno
 *
 * Return:
 *  SBC_OK - Success
 *
 */
scb_err_t scb_destroy(char *name, int *err);

/* void scb_strerror(scb_err_t err, int ret, char *msg)
 *
 * Parameters:
 *  err - In -
 *  ret - In -
 *  msg - Out -
 *
 * Return:
 *  void
 */
void scb_strerror(scb_err_t err, int ret, char *msg);

#endif
