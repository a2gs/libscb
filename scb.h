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

typedef struct _scb_t{
	sem_t empty;
	sem_t full;
	sem_t buffCtrl;

	uint16_t head;
	uint16_t tail;

	uint16_t dataTotal; /* Total of elements (capacity) */
	size_t dataElementSz; /* Size of one element */
	void *data;
}scb_t;

#define SCB_OK         (0)
#define SCB_ERR_CREATE (1)

int scb_create(char *name, uint16_t totalElements, size_t sizeElements, scb_t *ctx);
int scb_attach(scb_t *ctx, char *name);

int scb_get(scb_t *ctx);
int scb_put(scb_t *ctx);

int scb_iterator_create(scb_t *ctx);
int scb_iterator_get(scb_t *ctx);

int scb_destroy(scb_t *ctx);

#endif
