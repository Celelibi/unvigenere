#ifndef FILTERED_STRING_H__
#define FILTERED_STRING_H__

#include <sys/types.h>

#include "charset.h"



struct fs_ctx {
	char *str;
	const struct charset *charset;
	size_t len;
	char *norm;
};



/* initialize ctx with the given information */
void fs_init(struct fs_ctx *ctx, char *str, const struct charset *charset);

void fs_fini(struct fs_ctx *ctx);

/*
 * Convert a logical char index into a physical index.
 * i.e. you'll get the index in str of the n-th character that belong the the
 * given charset. Return -1 if it doesn't exists.
 */
ssize_t fs_pidx(const struct fs_ctx *ctx, size_t n);

/*
 * Return the caracter that match the charset just next to the given physical
 * index.
 */
ssize_t fs_next(const struct fs_ctx *ctx, size_t n);

/*
 * Get directly the n-th character that belong the the charset.
 * This function is equivalent to str[fs_pidx(n)] in the usual case.
 * It returns nul char '\0' if n is not in the string.
 */
char fs_char(const struct fs_ctx *ctx, size_t n);

/*
 * Remplace all the non-filtered chars in the string with these ones.
 */
void fs_replace(struct fs_ctx *ctx, const char *norm);

/*
 * Function to call to tell the filtered string a given char has been modified.
 * n is the physical index of the modified char.
 */
void fs_update(struct fs_ctx *ctx, size_t n);

/*
 * This method tells to re-parse the full string.
 */
void fs_update_all(struct fs_ctx *ctx);

#endif
