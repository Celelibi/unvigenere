#ifndef ARRAY_H__
#define ARRAY_H__

#include <sys/types.h>

#define ARRAY_DECL(type, name) \
	type *name;             \
	size_t name ## _mem;   \
	size_t name ## _size

#define ARRAY_ALLOC(name, size)                                      \
	do {                                                         \
		name ## _mem = (size);                               \
		name ## _size = 0;                                   \
		name = array_alloc((name ## _mem), sizeof(*(name))); \
	} while (0)

#define ARRAY_GROW(name) \
	name = array_grow((name), &(name ## _mem), sizeof(*(name)))

#define ARRAY_APPEND(name, elem)                                      \
	name = array_append((name), &(name ## _mem), sizeof(*(name)), \
	                   &(name ## _size), &(elem))

#define ARRAY_FREE(name)             \
	do {                         \
		array_free(name);    \
		name = NULL;         \
		(name ## _mem) = 0;  \
		(name ## _size) = 0; \
	} while (0)

/* Allocate a new array of array_mem elements of elemsize bytes. */
void *array_alloc(size_t array_mem, size_t elemsize);

/* Double the buffer size. */
void *array_grow(void *array, size_t *array_mem, size_t elemsize);

/* Append an element to an array. */
void *array_append(void *array, size_t *array_mem, size_t elemsize,
                   size_t *array_size, void *elem);

void array_free(void *array);

#endif
