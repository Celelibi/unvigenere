#ifndef KASISKI_H__
#define KASISKI_H__

/*
 * This module perform a Kasiski analysis on a given text.
 * A Kasiski analysis is a kind of discreet autocorrelation on a text.
 * It consist in comparing the text with a copy of itself shifted by a given
 * offset. And then the comparison is done by counting the substring that match.
 */


#include <sys/types.h>

#include "filtered_string.h"




struct kasiski {
	const struct fs_ctx *str;
	size_t minlen;

	/* ka_analyze will fill this array so that score[klen] is the number of
	 * substring of str that has been found at n*klen distance. */
	size_t *score;
};



/* Initialize a kasiski structure. minlen is the minimal length of the
 * substrings to match. */
void ka_init(struct kasiski *k, const struct fs_ctx *str, size_t minlen);

/* Deinitialize a kasiski structure. */
void ka_fini(struct kasiski *k);

/* Perform a Kasiski analysis. */
void ka_analyze(struct kasiski *k);

#endif
