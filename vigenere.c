#include <stdlib.h>
#include <string.h>

#include "misc.h"
#include "array.h"
#include "filtered_string.h"
#include "vigenere.h"




void vig_init(struct vigenere *vig, struct fs_ctx *str) {
	memset(vig, 0, sizeof(*vig));
	vig->str = str;
}



void vig_fini(struct vigenere *vig) {
	if (vig->charsets == NULL)
		return;

	ARRAY_FREE(vig->charsets);
}



void vig_add_charset(struct vigenere *vig, const char *charset) {
	/* Allocate the array if needed. */
	if (vig->charsets == NULL)
		ARRAY_ALLOC(vig->charsets, 1);

	if (vig->charsets_size > 0 && strlen(charset) != vig->charset_len)
		custom_error("vig_add_charset: "
		             "All the charsets must have the same length");

	/* TODO: check the charsets don't overlap. */
	/* TODO: check the charsets match vig->str->charset */

	if (vig->charsets_size == 0)
		vig->charset_len = strlen(charset);

	ARRAY_APPEND(vig->charsets, charset);
}



static int char_num(struct vigenere *vig, char c) {
	size_t i;

	for (i = 0; i < vig->charsets_size; i++) {
		char *p = strchr(vig->charsets[i], c);
		if (p != NULL)
			return p - vig->charsets[i];
	}

	return -1;
}



static char shift_char(struct vigenere *vig, char c, int s) {
	size_t i;

	for (i = 0; i < vig->charsets_size; i++) {
		char *p = strchr(vig->charsets[i], c);
		if (p != NULL) {
			int n = p - vig->charsets[i] + s;
			return vig->charsets[i][n % vig->charset_len];
		}
	}

	return -1;
}



static char unshift_char(struct vigenere *vig, char c, int s) {
	size_t i;

	for (i = 0; i < vig->charsets_size; i++) {
		char *p = strchr(vig->charsets[i], c);
		if (p != NULL) {
			int n = p - vig->charsets[i] - s + vig->charset_len;
			return vig->charsets[i][n % vig->charset_len];
		}
	}

	return -1;
}



static size_t key_length(struct vigenere *vig, const char *key) {
	size_t len = 0;

	while (*key != '\0') {
		if (char_num(vig, *key) != -1)
			len++;

		key++;
	}

	return len;
}



/* TODO: refactor vig_encrypt and vig_decrypt */
void vig_encrypt(struct vigenere *vig, const char *key) {
	size_t klen, ki;
	char c;

	klen = key_length(vig, key);

	for (ki = 0; key[ki] != '\0'; ki++) {
		size_t idx;
		int shift = char_num(vig, key[ki]);

		if (shift == -1)
			continue;

		idx = ki;
		do {
			ssize_t pidx;

			pidx = fs_pidx(vig->str, idx);
			if (pidx == -1)
				break;

			c = vig->str->str[pidx];
			c = shift_char(vig, c, shift);
			vig->str->str[pidx] = c;

			idx += klen;
		} while (c != '\0');
	}
}



void vig_decrypt(struct vigenere *vig, const char *key) {
	size_t klen, ki;
	char c;

	klen = key_length(vig, key);

	for (ki = 0; key[ki] != '\0'; ki++) {
		size_t idx;
		int shift = char_num(vig, key[ki]);

		if (shift == -1)
			continue;

		idx = ki;
		do {
			ssize_t pidx;

			pidx = fs_pidx(vig->str, idx);
			if (pidx == -1)
				break;

			c = vig->str->str[pidx];
			c = unshift_char(vig, c, shift);
			vig->str->str[pidx] = c;

			idx += klen;
		} while (c != '\0');
	}
}
