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
	scb_ctrl_t ctrl;
	void *data;
}scb_t;

typedef struct _scb_iter_t{
	uint16_t it;
}scb_iter_t;

typedef enum{
	SCB_OK = 0,
	SCB_SHMEM,
	SCB_FTRUNC,
	SCB_SEMPH,
	SCB_MMAP
}scb_err_t;

scb_err_t scb_create(char *name, uint16_t totalElements, size_t sizeElements, scb_t **ctx, int *err);
scb_err_t scb_attach(scb_t **ctx, char *name, int *err);

scb_err_t scb_get_block(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src));
/* scb_err_t scb_get_nonblock(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src)); */

scb_err_t scb_put_block(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src));
/* scb_err_t scb_put(scb_t *ctx, void *element, void *(*copyElement)(void *dst, const void *src)); */

scb_err_t scb_getInfo(char *name, scb_t *inf, int *err);

scb_err_t scb_iterator_create(scb_t *ctx, scb_iter_t *ctxIter);
scb_err_t scb_iterator_get(scb_t *ctx, scb_iter_t *ctxIter, void *data);

scb_err_t scb_destroy(scb_t *ctx, int *err);

void scb_strerror(scb_err_t err, int ret, char *msg);

#endif
