#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "misc.h"
#include "kasiski.h"



void ka_init(struct kasiski *k, const char *str, size_t minlen) {
	memset(k, 0, sizeof(*k));
	k->str = str;
	k->str_len = strlen(str);
	k->minlen = minlen;

	k->score = malloc(k->str_len * sizeof(*k->score));
	if (k->score == NULL)
		system_error("malloc");

	memset(k->score, 0, k->str_len * sizeof(*k->score));
}



void ka_fini(struct kasiski *k) {
	free(k->score);
	memset(k, 0, sizeof(*k));
}



/* Count the number of substring of length k->minlen that are present in the
 * string at an offset off from each other. */
static size_t analyze_offset_count(struct kasiski *k, size_t off) {
	size_t count = 0;
	ssize_t match_start = -1;
	size_t i;

	for (i = off; k->str[i] != '\0'; i++) {
		char c1 = k->str[i];
		char c2 = k->str[i - off];

		/* Were in a substring match and it just ended. */
		if (match_start != -1 && c1 != c2) {
			size_t sublen = i - match_start;
			if (sublen >= k->minlen)
				count++;

			match_start = -1;
		}

		/* Just entered a substring match. */
		if (match_start == -1 && c1 == c2)
			match_start = i;
	}

	return count;
}



static void analyze_offset(struct kasiski *k, size_t off) {
	size_t count;
	size_t i;

	count = analyze_offset_count(k, off);

	/* There is no need to test every integer greater than off/2.
	 * off can't be divisible be anything between off/2 and off. */
	for (i = 1; i <= off / 2; i++) {
		if (count % i == 0)
			k->score[i] += count;
	}
	k->score[off] += count;
}



void ka_analyze(struct kasiski *k) {
	size_t i;

	for (i = k->minlen; i < k->str_len; i++)
		analyze_offset(k, i);
}
