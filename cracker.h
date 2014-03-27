#ifndef CRACKER_H__
#define CRACKER_H__

/*
 * This module coordinate all the analysis modules to crack a Vigenère cipher.
 * Namely, it curently uses:
 * - Multi-frequency-analysis
 * - Kasiski analysis
 *
 * And will use:
 * - Friedman test
 * - Index of coincidence analysis
 * - Wordlist attack
 * - Autocorrelation (with or without a FFT)
 * - Fourier Transform variation for text
 *
 * This module's API (will) also give access various statistics about the text
 * being cracked.
 */

#include <sys/types.h>

#include "filtered_string.h"
#include "mfreq_analysis.h"
#include "kasiski.h"



struct cracker {
	const struct fs_ctx *str;
	const char *keycharset;
	size_t keycharset_len;
	size_t klen;
	char *key;

	/* Argument to be given to ka_init */
	size_t ka_minlen;

	struct kasiski ka;
	int ka_done;

	struct mfreq mfa;
	int mfa_done;
};



/*
 * Initialize a struct cracker. keycharset is the charset from which to build
 * the key.
 */
void ck_init(struct cracker *state, struct fs_ctx *str, const char *keycharset);

/* Free everything one of the ck_* function allocated. */
void ck_fini(struct cracker *state);

/* Set the key length. */
void ck_set_length(struct cracker *state, size_t len);

/* Crack the key length. */
void ck_length(struct cracker *state);

/* Crack the password using frequency analysis. */
void ck_freq(struct cracker *state);

/* Crack the Vigenère cipher using all the implemented techniques. */
void ck_crack(struct cracker *state);

#endif
