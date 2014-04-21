#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "misc.h"
#include "charset.h"
#include "freq.h"




/* Initialize a struct freq. */
void freq_init(struct freq *f, const struct charset *charset) {
	memset(f, 0, sizeof(*f));

	f->charset = charset;
	f->freq = malloc(sizeof(*f->freq) * charset->length);
	if (f->freq == NULL)
		system_error("malloc");

	memset(f->freq, 0, sizeof(*f->freq) * charset->length);
}



/* Deinitialize a struct freq. */
void freq_fini(struct freq *f) {
	free(f->freq);
	memset(f, 0, sizeof(*f));
}



/*
 * Compute the frequency of the string str with repect to the charset given in
 * the initialization, but only take one character every n characters of str.
 */
void freq_compute_stride(struct freq *f, const char *str, size_t n) {
	/* Use a temporary array "count" so that float errors won't disturb the
	 * result. */
	size_t *count;
	size_t i;
	size_t total;
	size_t nlen;
	const struct charset *cs; /* shorthand */

	cs = f->charset;
	nlen = strlen(str);

	count = malloc(sizeof(*count) * cs->length);
	if (count == NULL)
		system_error("malloc");

	memset(count, 0, sizeof(*count) * cs->length);

	for (i = 0; i < nlen; i += n) {
		int o = cs_ord(cs, str[i]);

		if (o == -1)
			continue;

		count[o]++;
	}

	total = 0;
	for (i = 0; i < cs->length; i++)
		total += count[i];

	for (i = 0; i < cs->length; i++)
		f->freq[i] = 100.0 * count[i] / (float)total;

	free(count);
}



/*
 * Compute the frequency of the string str with repect to the charset given in
 * the initialization.
 */
void freq_compute(struct freq *f, const char *str) {
	freq_compute_stride(f, str, 1);
}
