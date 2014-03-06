#ifndef FILTERED_STRING_H__
#define FILTERED_STRING_H__

#include <sys/types.h>

#define FS_CHARSET_LOWER "abcdefghijklmnopqrstuvwxyz"
#define FS_CHARSET_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define FS_CHARSET_ALPHA FS_CHARSET_LOWER FS_CHARSET_UPPER



struct fs_ctx {
	char *str;
	const char *charset;
	size_t len;
};



/* initialize ctx with the given information */
void fs_init(struct fs_ctx *ctx, char *str, const char *charset);

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
 * Function to call to tell the filtered string a given char has been modified.
 * n is the physical index of the modified char.
 */
void fs_update(struct fs_ctx *ctx, size_t n);

/*
 * This method tells to re-parse the full string.
 */
void fs_update_all(struct fs_ctx *ctx);

#endif
