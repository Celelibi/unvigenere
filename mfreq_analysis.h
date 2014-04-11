#ifndef MFREQ_ANA_H__
#define MFREQ_ANA_H__


/*
 * This module perform a multi frequency analysis on a given text.
 * It currently only support the 26 letters of the alphabet and doesn't take
 * care of the letter case.
 */


#include <sys/types.h>

#include "array.h"


/* A type for the static 26 letters analysis. */
typedef float freq_t[26];

struct mfreq {
	const char *str;
	size_t klen;

	/* One table for every key letter. */
	freq_t *freq;

	/* Best shifts. */
	size_t *shift;
};



/* Initialize a struct mfreq. */
void mfa_init(struct mfreq *mfa, const char *str, size_t klen);

/* Deinitialize a struct mfreq. */
void mfa_fini(struct mfreq *mfa);

/* Compute the frequencies and the best shifts. */
void mfa_analyze(struct mfreq *mfa);

/* TODO: Change the shift of the n-th element and return the new distance to
 * language frequencies. */

#endif
