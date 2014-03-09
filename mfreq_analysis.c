#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <float.h>

#include "misc.h"
#include "array.h"
#include "mfreq_analysis.h"



/* The frequencies for letters A..Z for several pre-defined languages. */
static freq_t freq_en = {
	8.08, 1.67, 3.18, 3.99, 12.56, 2.17, 1.80, 5.27, 7.24, 0.14, 0.63, 4.04,
	2.60, 7.38, 7.47, 1.91, 0.09, 6.42, 6.59, 9.15, 2.79, 1.00, 1.89, 0.21,
	1.65, 0.07
};

/*static freq_t freq_fr = {
	9.2,  1.02, 2.64, 3.39, 15.87, 0.95, 1.04, 0.77, 8.41, 0.89, 0.00, 5.34,
	3.24, 7.15, 5.14, 2.86, 1.06, 6.46, 7.90, 7.26, 6.24, 2.15, 0.00, 0.30,
	0.24, 0.32
};*/






void mfa_init(struct mfreq *mfa, const struct fs_ctx *str, size_t klen) {
	memset(mfa, 0, sizeof(*mfa));

	mfa->str = str;
	mfa->klen = klen;

	mfa->freq = malloc(sizeof(*mfa->freq) * klen);
	if (mfa->freq == NULL)
		system_error("malloc");

	mfa->shift = malloc(sizeof(*mfa->shift) * klen);
	if (mfa->shift == NULL)
		system_error("malloc");
}



void mfa_fini(struct mfreq *mfa) {
	free(mfa->shift);
	free(mfa->freq);
	memset(mfa, 0, sizeof(*mfa));
}



/*
 * Return an uppercase letter.
 * FIXME: This function is not foreign language friendly and not UTF-8 compliant
 * at all. */
static char sanitize_char(char c) {
	c = toupper(c);

	/* Not charset friendly at all. */
	if (strchr("ÁÀÂÄÃ", c) != NULL)
		return 'A';

	if (strchr("ÉÈÊËẼ", c) != NULL)
		return 'E';

	if (strchr("ÍÌîï", c) != NULL)
		return 'I';

	if (strchr("ÓÒÔÖÕ", c) != NULL)
		return 'O';

	if (strchr("ÚÙÛÜ", c) != NULL)
		return 'U';

	if (strchr("ÝŶŸ", c) != NULL)
		return 'Y';

	return c;
}



/* Compute the frequency table for a given offset. */
static void frequency_offset(struct mfreq *mfa, size_t off) {
	/* Use size_t type to avoid float cancellation when doing "+1". */
	size_t count[26];
	size_t i;
	size_t total;
	char c;

	memset(&count, 0, sizeof(count));

	for (i = off; (c = fs_char(mfa->str, i)) != '\0'; i += mfa->klen) {
		c = sanitize_char(c);

		if (c >= 'A' && c <= 'Z')
			count[c - 'A']++;
	}

	total = 0;
	for (i = 0; i < 26; i++)
		total += count[i];

	for (i = 0; i < 26; i++)
		mfa->freq[off][i] = 100.0 * count[i] / (float)total;
}



static float freq_distance_shift(freq_t lang_freq, freq_t freq,
                                 size_t shift) {

	float d = 0;
	size_t i;

	/* Just compute the euclidian distance. */
	for (i = 0; i < ARRAY_LENGTH(freq_en); i++) {
		float diff = freq[i] - lang_freq[(i + shift) % 26];
		d += diff * diff;
	}

	return d;
}



/* Compute the best shift for a given array of frequencies.
 * The "best shift" is defined as the one that makes the frequency the closer to
 * the wanted frequencies. */
static size_t best_shift(freq_t lang_freq, freq_t f) {
	float bd = FLT_MAX;
	size_t bs;
	size_t i;

	for (i = 0; i < 26; i++) {
		float d = freq_distance_shift(lang_freq, f, i);
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

	memset(mfa->freq, 0, sizeof(*mfa->freq) * mfa->klen);
	memset(mfa->freq, 0, sizeof(*mfa->shift) * mfa->klen);

	for (i = 0; i < mfa->klen; i++) {
		frequency_offset(mfa, i);
		mfa->shift[i] = best_shift(freq_en, mfa->freq[i]);
	}
}