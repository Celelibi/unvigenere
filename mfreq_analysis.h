#ifndef MFREQ_ANA_H__
#define MFREQ_ANA_H__


/*
 * This module perform a multi frequency analysis on a given text.
 */


#include <sys/types.h>

#include "charset.h"
#include "freq.h"


/* The frequencies for letters A..Z for several pre-defined languages. */
extern float freq_en[];
extern float freq_fr[];




struct mfreq {
	const char *str;
	size_t klen;

	const struct charset *charset;
	const float *reffreq;

	/* One table for every key letter. */
	struct freq *freq;

	/* Best shifts. */
	size_t *shift;
};



/* Initialize a struct mfreq. */
void mfa_init(struct mfreq *mfa, const char *str, size_t klen,
              const struct charset *charset, const float *reffreq);

/* Deinitialize a struct mfreq. */
void mfa_fini(struct mfreq *mfa);

/* Compute the frequencies and the best shifts. */
void mfa_analyze(struct mfreq *mfa);

/* TODO: Change the shift of the n-th element and return the new distance to
 * language frequencies. */

#endif
