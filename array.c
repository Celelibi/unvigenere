#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "misc.h"
#include "array.h"


void *array_alloc(size_t array_mem, size_t elemsize) {
	void *array;

	assert(array_mem > 0);

	/* TODO: check for integer overflow */
	array = malloc(array_mem * elemsize);
	if (array == NULL)
		system_error("array malloc");

	return array;
}



void *array_grow(void *array, size_t *array_mem, size_t elemsize) {
	*array_mem *= 2;
	array = realloc(array, *array_mem * elemsize);
	if (array == NULL)
		system_error("array realloc");

	return array;
}



void *array_append(void *array, size_t *array_mem, size_t elemsize,
                   size_t *array_size, void *elem) {
	char *carray;

	if (*array_size >= *array_mem)
		array = array_grow(array, array_mem, elemsize);

	carray = array;
	memcpy(carray + *array_size * elemsize, elem, elemsize);
	(*array_size)++;

	return array;
}



void array_free(void *array) {
	free(array);
}
