#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "misc.h"
#include "filtered_string.h"


/*
 * TODO: implement this module as a balanced tree of intervals so that the
 * character lookup is faster.
 */

void fs_init(struct fs_ctx *ctx, char *str, const char *charset) {
	size_t nlen = 0;
	ssize_t i;

	memset(ctx, 0, sizeof(*ctx));
	ctx->str = str;
	ctx->charset = charset;
	ctx->len = strlen(str);

	for (i = fs_pidx(ctx, 0); i >= 0; i = fs_next(ctx, i))
		nlen++;

	ctx->norm = malloc((nlen + 1) * sizeof(*ctx->norm));
	if (ctx->norm == NULL)
		system_error("malloc");

	ctx->norm[nlen] = '\0';

	/* Will reconstruct the content of ctx->norm based on ctx->str. */
	fs_update_all(ctx);
}



void fs_fini(struct fs_ctx *ctx) {
	free(ctx->norm);
	memset(ctx, 0, sizeof(*ctx));
}



ssize_t fs_pidx(const struct fs_ctx *ctx, size_t n) {
	const char *p;

	p = strpbrk(ctx->str, ctx->charset);

	while (n > 0 && p != NULL) {
		p = strpbrk(p + 1, ctx->charset);
		n--;
	}

	if (p == NULL)
		return -1;

	return p - ctx->str;
}



ssize_t fs_next(const struct fs_ctx *ctx, size_t n) {
	const char *p;

	if (n >= ctx->len)
		return -1;

	p = strpbrk(ctx->str + n + 1, ctx->charset);

	if (p == NULL)
		return -1;

	return p - ctx->str;
}



char fs_char(const struct fs_ctx *ctx, size_t n) {
	ssize_t idx = fs_pidx(ctx, n);
	if (idx == -1)
		return '\0';

	return ctx->str[idx];
}



void fs_replace(struct fs_ctx *ctx, const char *norm) {
	ssize_t i;

	if (strlen(norm) != strlen(ctx->norm))
		custom_error("fs_replace called with unapplyable new string");

	strcpy(ctx->norm, norm);

	for (i = fs_pidx(ctx, 0); i >= 0; i = fs_next(ctx, i))
		ctx->str[i] = *norm++;
}



void fs_update(struct fs_ctx *ctx, size_t n) {
	ctx->norm[fs_pidx(ctx, n)] = ctx->str[n];
}



void fs_update_all(struct fs_ctx *ctx) {
	ssize_t i;
	size_t j = 0;

	for (i = fs_pidx(ctx, 0); i >= 0; i = fs_next(ctx, i))
		ctx->norm[j++] = ctx->str[i];
}
