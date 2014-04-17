#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "array.h"
#include "charset.h"


/* Initialize an empty charset. */
void cs_init(struct charset *cs) {
	memset(cs, 0, sizeof(*cs));
	ARRAY_ALLOC(cs->chars, 2);
}



/* Deinitialize a charset. */
void cs_fini(struct charset *cs) {
	size_t i;

	for (i = 0; i < cs->chars_size; i++)
		free(cs->chars[i]);

	ARRAY_FREE(cs->chars);

	memset(cs, 0, sizeof(*cs));
}



/* Add a list of chars to the charset. */
void cs_add(struct charset *cs, const char *str) {
	char *cpy;
	size_t len;

	len = strlen(str);
	cpy = strdup(str);

	if (cs->chars_size == 0)
		cs->length = len;

	if (len != cs->length)
		custom_error("cs_add: "
		             "All the charsets must have the same length");

	ARRAY_APPEND(cs->chars, cpy);
}



/* This is just function that does the work of cs_belong, and a part of the work
 * of cs_equiv and cs_norm all at the same time. */
static int find_char(const struct charset *cs, char c,
                      size_t *stridx, size_t *pos) {
	size_t i;

	for (i = 0; i < cs->chars_size; i++) {
		char *p = strchr(cs->chars[i], c);

		/* Yay, found it! */
		if (p != NULL) {
			if (stridx)
				*stridx = i;
			if (pos)
				*pos = p - cs->chars[i];

			return 1;
		}
	}

	return 0;
}



/* Ask whether a character belong to the charset. Return 1 if yes, 0 if no. */
int cs_belong(const struct charset *cs, char c) {
	return find_char(cs, c, NULL, NULL);
}



/* Ask whether two characters are equivalents. */
int cs_equiv(const struct charset *cs, char c1, char c2) {
	size_t c1_pos;
	size_t c2_pos;

	/* If you can't even find c1, it's not equivalent to c2! */
	if (!find_char(cs, c1, NULL, &c1_pos))
		return 0;

	/* If you can't even find c2, it's not equivalent to c1! */
	if (!find_char(cs, c2, NULL, &c2_pos))
		return 0;

	/* They're equivalent only if they are at the same position. */
	return c1_pos == c2_pos;
}



/* Retrive the normalized version of a character. That may be any character
 * give, it is guaranteed to be always the same. Return c if the character do
 * not belong to the charset. */
char cs_norm(const struct charset *cs, char c) {
	size_t pos;

	if (!find_char(cs, c, NULL, &pos))
		return c;

	return cs->chars[0][pos];
}



/* Equivalent to strpbrk with a struct charset as set of accepted characters.
 * /!\ The arguments are inverted w.r.t strpbrk to keep consistent with the
 * other functions of this module. */
char *cs_strpbrk(const struct charset *cs, const char *str) {
	char *min_pos = NULL;
	size_t i;

	/* Basically, we search the first character that corresponds to one of
	 * the charset strings. */

	for (i = 0; i < cs->chars_size; i++) {
		char *pos = strpbrk(str, cs->chars[i]);

		if (pos == NULL)
			continue;

		if (min_pos == NULL || pos < min_pos)
			min_pos = pos;
	}

	return min_pos;
}
