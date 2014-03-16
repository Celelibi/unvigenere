#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

#include "misc.h"
#include "filtered_string.h"
#include "mfreq_analysis.h"
#include "cracker.h"



/* Initialize a struct cracker. */
void ck_init(struct cracker *state, struct fs_ctx *str, const char *keycharset) {
	memset(state, 0, sizeof(*state));
	state->str = str;

	state->keycharset = keycharset;
	state->keycharset_len = strlen(state->keycharset);
}



/* Free everything one of the ck_* function allocated */
void ck_fini(struct cracker *state) {
	if (state->key != NULL)
		free(state->key);

	if (state->mfa_done)
		mfa_fini(&state->mfa);

	memset(state, 0, sizeof(*state));
}



void ck_set_length(struct cracker *state, size_t len) {
	state->klen = len;

	if (state->key != NULL)
		free(state->key);

	state->key = malloc((len + 1) * sizeof(*state->key));
	if (state->key == NULL)
		system_error("malloc");

	memset(state->key, 0, (len + 1) * sizeof(*state->key));
}



void ck_length(struct cracker *state) {
	(void)state;
	custom_error("Key length cracking is not implemented yet.");
}



static void key_from_mfa_shift(struct cracker *state) {
	size_t i;

	assert(state->mfa_done);

	for (i = 0; i < state->klen; i++) {
		/* Substract because we're undoing a Vigenère. */
		size_t j = state->keycharset_len - state->mfa.shift[i];
		state->key[i] = state->keycharset[j % state->keycharset_len];
	}
}



void ck_freq(struct cracker *state) {
	if (state->klen == 0)
		custom_error("Call either ck_length or ck_set_length "
		             "before calling ck_freq.");

	/* Restart the multi-freq analysis if specifically asked to. */
	if (state->mfa_done)
		mfa_fini(&state->mfa);

	mfa_init(&state->mfa, state->str, state->klen);
	state->mfa_done = 1;

	mfa_analyze(&state->mfa);
	key_from_mfa_shift(state);
}



void ck_crack(struct cracker *state) {
	if (state->klen == 0)
		ck_length(state);

	if (!state->mfa_done)
		ck_freq(state);
}