#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "misc.h"
#include "charset.h"
#include "mfreq_analysis.h"



float freq_en[] = {
	8.08, 1.67, 3.18, 3.99, 12.56, 2.17, 1.80, 5.27, 7.24, 0.14, 0.63, 4.04,
	2.60, 7.38, 7.47, 1.91, 0.09, 6.42, 6.59, 9.15, 2.79, 1.00, 1.89, 0.21,
	1.65, 0.07
};

float freq_fr[] = {
	9.2,  1.02, 2.64, 3.39, 15.87, 0.95, 1.04, 0.77, 8.41, 0.89, 0.00, 5.34,
	3.24, 7.15, 5.14, 2.86, 1.06, 6.46, 7.90, 7.26, 6.24, 2.15, 0.00, 0.30,
	0.24, 0.32
};






void mfa_init(struct mfreq *mfa, const char *str, size_t klen,
              const struct charset *charset, const float *freq) {
	size_t i;

	memset(mfa, 0, sizeof(*mfa));

	mfa->str = str;
	mfa->klen = klen;
	mfa->charset = charset;

	if (freq != NULL)
		mfa->reffreq = freq;
	else
		mfa->reffreq = freq_en;

	mfa->freq = malloc(sizeof(*mfa->freq) * klen);
	if (mfa->freq == NULL)
		system_error("malloc");

	for (i = 0; i < klen; i++) {
		mfa->freq[i] = malloc(sizeof(*mfa->freq[i]) * charset->length);
		if (mfa->freq[i] == NULL)
			system_error("malloc");
	}

	mfa->shift = malloc(sizeof(*mfa->shift) * klen);
	if (mfa->shift == NULL)
		system_error("malloc");
}



void mfa_fini(struct mfreq *mfa) {
	size_t i;
	free(mfa->shift);

	for (i = 0; i < mfa->klen; i++)
		free(mfa->freq[i]);

	free(mfa->freq);

	memset(mfa, 0, sizeof(*mfa));
}



/* Compute the frequency table for a given offset. */
static void frequency_offset(struct mfreq *mfa, size_t off) {
	/* Use size_t type to avoid float cancellation when doing "+1". */
	size_t *count;
	size_t i;
	size_t total;
	size_t nlen;
	const struct charset *cs; /* shorthand */

	cs = mfa->charset;
	nlen = strlen(mfa->str);

	count = malloc(sizeof(*count) * cs->length);
	if (count == NULL)
		system_error("malloc");

	memset(count, 0, sizeof(*count) * cs->length);

	for (i = off; i < nlen; i += mfa->klen) {
		int o = cs_ord(cs, mfa->str[i]);

		if (o == -1)
			continue;

		count[o]++;
	}

	total = 0;
	for (i = 0; i < cs->length; i++)
		total += count[i];

	for (i = 0; i < cs->length; i++)
		mfa->freq[off][i] = 100.0 * count[i] / (float)total;

	free(count);
}



static float freq_distance_shift(const struct mfreq *mfa, size_t off,
                                 size_t shift) {

	float d = 0;
	const struct charset *cs; /* Shorthand */
	const float *lang_freq; /* shorthand */
	const float *freq; /* shorthand */
	size_t i;

	cs = mfa->charset;
	lang_freq = mfa->reffreq;
	freq = mfa->freq[off];

	/* Just compute the euclidian distance. */
	for (i = 0; i < cs->length; i++) {
		float diff = freq[i] - lang_freq[(i + shift) % cs->length];
		d += diff * diff;
	}

	return d;
}



/* Compute the best shift for a given array of frequencies.
 * The "best shift" is defined as the one that makes the frequency the closer to
 * the wanted frequencies. */
static size_t best_shift(const struct mfreq *mfa, size_t n) {
	float bd = FLT_MAX;
	size_t bs;
	size_t i;

	for (i = 0; i < mfa->charset->length; i++) {
		float d = freq_distance_shift(mfa, n, i);
		if (d < bd) {
			bd = d;
			bs = i;
		}
	}

	return bs;
}



/* Compute the frequencies and the best shifts. */
void mfa_analyze(struct mfreq *mfa) {
	size_t i;

	for (i = 0; i < mfa->klen; i++)
		memset(mfa->freq[i], 0, sizeof(*mfa->freq) * mfa->klen);

	memset(mfa->shift, 0, sizeof(*mfa->shift) * mfa->klen);

	for (i = 0; i < mfa->klen; i++) {
		frequency_offset(mfa, i);
		mfa->shift[i] = best_shift(mfa, i);
	}
}
