#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "charset.h"
#include "filtered_string.h"
#include "vigenere.h"




static char shift_char(struct fs_ctx *str, char c, int s) {
	size_t sidx, pos;
	const struct charset *cs; /* shorthand */

	cs = str->charset;

	if (!cs_find_char(cs, c, &sidx, &pos))
		return -1;

	/* This double trick allow to ensure that s is always positive and less
	 * than cs->length. Even if s greatly negative. */
	s %= (int)cs->length;
	s = (s + cs->length) % cs->length;

	pos = (pos + s) % cs->length;

	return cs->chars[sidx][pos];
}



/* TODO: refactor vig_encrypt and vig_decrypt */
void vig_encrypt(struct fs_ctx *str, const char *key) {
	struct fs_ctx fskey;
	size_t klen, i;
	char *ntext;

	/* I know filtered_string won't modify key. */
	fs_init(&fskey, (char *)key, str->charset);
	key = fskey.norm;
	klen = strlen(key);

	/* Duplicate str->norm so that the struct str isn't transiently
	 * inconsistent. */
	ntext = strdup(str->norm);

	i = 0;
	while (ntext[i] != '\0') {
		int shift = cs_ord(str->charset, key[i % klen]);
		ntext[i] = shift_char(str, ntext[i], shift);
		i++;
	}

	fs_replace(str, ntext);
	free(ntext);
	fs_fini(&fskey);
}



void vig_decrypt(struct fs_ctx *str, const char *key) {
	struct fs_ctx fskey;
	size_t klen, i;
	char *ntext;

	/* I know filtered_string won't modify key. */
	fs_init(&fskey, (char *)key, str->charset);
	key = fskey.norm;
	klen = strlen(key);

	/* Duplicate str->norm so that the struct str isn't transiently
	 * inconsistent. */
	ntext = strdup(str->norm);

	i = 0;
	while (ntext[i] != '\0') {
		int shift = cs_ord(str->charset, key[i % klen]);
		ntext[i] = shift_char(str, ntext[i], -shift);
		i++;
	}

	fs_replace(str, ntext);
	free(ntext);
	fs_fini(&fskey);
}
