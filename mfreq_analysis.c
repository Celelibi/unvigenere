#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "misc.h"
#include "charset.h"
#include "freq.h"
#include "mfreq_analysis.h"



float freq_en[] = {
	0.0808, 0.0167, 0.0318, 0.0399, 0.1256, 0.0217, 0.0180, 0.0527, 0.0724,
	0.0014, 0.0063, 0.0404, 0.0260, 0.0738, 0.0747, 0.0191, 0.0009, 0.0642,
	0.0659, 0.0915, 0.0279, 0.0100, 0.0189, 0.0021, 0.0165, 0.0007
};

float freq_fr[] = {
	0.0920, 0.0102, 0.0264, 0.0339, 0.1587, 0.0095, 0.0104, 0.0077, 0.0841,
	0.0089, 0.0000, 0.0534, 0.0324, 0.0715, 0.0514, 0.0286, 0.0106, 0.0646,
	0.0790, 0.0726, 0.0624, 0.0215, 0.0000, 0.0030, 0.0024, 0.0032
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

	for (i = 0; i < klen; i++)
		freq_init(&mfa->freq[i], charset);

	mfa->shift = malloc(sizeof(*mfa->shift) * klen);
	if (mfa->shift == NULL)
		system_error("malloc");
}



void mfa_fini(struct mfreq *mfa) {
	size_t i;
	free(mfa->shift);

	for (i = 0; i < mfa->klen; i++)
		freq_fini(&mfa->freq[i]);

	free(mfa->freq);

	memset(mfa, 0, sizeof(*mfa));
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
	freq = mfa->freq[off].freq;

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

	memset(mfa->shift, 0, sizeof(*mfa->shift) * mfa->klen);

	for (i = 0; i < mfa->klen; i++) {
		freq_compute_stride(&mfa->freq[i], mfa->str + i, mfa->klen);
		mfa->shift[i] = best_shift(mfa, i);
	}
}
