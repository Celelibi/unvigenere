#ifndef FREQ_H__
#define FREQ_H__

/*
 * Compute the frequency of some characters from a string with respect to a
 * charset.
 */

#include <sys/types.h>

#include "charset.h"


struct freq {
	const struct charset *charset;
	float *freq;
};



/* Initialize a struct freq. */
void freq_init(struct freq *f, const struct charset *charset);

/* Deinitialize a struct freq. */
void freq_fini(struct freq *f);

/*
 * Compute the frequency of the string str with repect to the charset given in
 * the initialization, but only take one character every n characters of str.
 */
void freq_compute_stride(struct freq *f, const char *str, size_t n);

/*
 * Compute the frequency of the string str with repect to the charset given in
 * the initialization.
 */
void freq_compute(struct freq *f, const char *str);


#endif
