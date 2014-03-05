#include <sys/types.h>
#include <string.h>

#include "filtered_string.h"


/*
 * TODO: implement this module as a balanced tree of intervals so that the
 * character lookup is faster.
 */

void fs_init(struct fs_ctx *ctx, const char *str, const char *charset) {
	ctx->str = str;
	ctx->charset = charset;
	ctx->len = strlen(str);
}



void fs_fini(struct fs_ctx *ctx) {
	/* Nothing to do for now. */
	(void)ctx;
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



void fs_update(struct fs_ctx *ctx, size_t n) {
	/* Nothing to do for a list implementation. */
	(void)ctx;
	(void)n;
}



void fs_update_all(struct fs_ctx *ctx) {
	/* Nothing to do for a list implementation. */
	(void)ctx;
}
