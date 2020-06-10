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

#define SCB_NAME_MAXSZ (100)

#define SCB_OK         (0)
#define SCB_ERR_CREATE (1)

typedef struct _scb_ctrl_t{
	sem_t empty;
	sem_t full;
	sem_t buffCtrl;

	uint16_t head;
	uint16_t tail;

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
}scb_iter_t;

int scb_create(char *name, uint16_t totalElements, size_t sizeElements, scb_t *ctx);
int scb_attach(scb_t *ctx, char *name);

int scb_get(scb_t *ctx);
int scb_put(scb_t *ctx);

int scb_iterator_create(scb_t *ctx, scb_iter_t *ctxIter);
int scb_iterator_get(scb_t *ctx, scb_iter_t *ctxIter, void *data);

int scb_destroy(scb_t *ctx);

#endif
