#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "getopthelp.h"
#include "cracker.h"
#include "filtered_string.h"
#include "vigenere.h"
#include "array.h"
#include "misc.h"



enum action {
	ACTION_ENCRYPT,
	ACTION_DECRYPT,
	ACTION_CRACK
};


/* Sequential numbers for option id. */
enum option_id {
	OPT_KASISKI_MIN_LENGTH = 256,
	OPT_SHOW_KASISKI_TABLE,
	OPT_SHOW_KASISKI_LENGTH,
	OPT_LAST
};


static const struct goh_option opt_desc[] = {
	{"input", 'i', GOH_ARG_REQUIRED, 'i',
		"Input file. May be - for stdin. Default to stdin."},
	{"output", 'o', GOH_ARG_REQUIRED, 'o',
		"Output file. May be - for stdout. Default to stdout."},
	{"encrypt", 'e', GOH_ARG_REFUSED, 'e',
		"Encrypt the data with the given key."},
	{"decrypt", 'd', GOH_ARG_REFUSED, 'd',
		"Decrypt the data with the given key."},
	{"key", 'k', GOH_ARG_REQUIRED, 'k',
		"Key used for encryption / decryption."},
	{"key-length", 'l', GOH_ARG_REQUIRED, 'l',
		"Length of the key to crack."},
	{"kasiski-min-length", 'm', GOH_ARG_REQUIRED, OPT_KASISKI_MIN_LENGTH,
		"Minimum length of a substring match for the kasiski method."},
	{"show-kasiski-table", '\0', GOH_ARG_REFUSED, OPT_SHOW_KASISKI_TABLE,
		"Show the score table for the kasiski method."},
	{"show-kasiski-length", '\0', GOH_ARG_REFUSED, OPT_SHOW_KASISKI_LENGTH,
		"Show the probable key length with respect to kasiski method."}
};





static void crack_ka_show_table(const struct cracker *ck) {
	size_t i;
	size_t nlen;

	printf("Kasiski score table:\n");

	nlen = strlen(ck->str->norm);
	for (i = 0; i < nlen; i++)
		printf("%lu: %lu\n", i, ck->ka.score[i]);
}




static const size_t *sort_score_helper_argument;



static int sort_score_helper(const void *arg1, const void *arg2) {
	const size_t *a = arg1;
	const size_t *b = arg2;
	const size_t *score = sort_score_helper_argument;

	return score[*b] - score[*a];
}



static void crack_ka_show_length(const struct cracker *ck) {
	size_t *ka_len;
	size_t i;
	size_t nlen;

	nlen = strlen(ck->str->norm);

	/*
	 * Build a table of index into ck->ka.score. They happen to be probable
	 * key length. The probability for the key to be length l is
	 * ck->ka.score[l].
	 */

	ka_len = malloc(nlen * sizeof(ka_len));
	if (ka_len == NULL)
		system_error("malloc");

	for (i = 0; i < nlen; i++)
		ka_len[i] = i;

	/* Sort that table according to the score. */
	sort_score_helper_argument = ck->ka.score;
	qsort(ka_len, nlen, sizeof(*ka_len), sort_score_helper);

	printf("Probable key length with respect to Kasiski attack:");

	/* Start at 1 because we know the length with the best score is length
	 * 1, which is not interesting. */
	for (i = 1; i < nlen; i++) {
		printf(" %lu", ka_len[i]);

		/* Only keep the length for which the score is not too far from
		 * the best choice. */
		if (ck->ka.score[ka_len[i]] < ck->ka.score[ka_len[1]] / 3)
			break;
	}
	printf("\n");

	free(ka_len);
}



/* There are too much arguments for the crack function and more are coming.
 * Let's just put them all in a struct. */
struct crack_args {
	struct fs_ctx *str;
	size_t klen;
	size_t ka_minlen;
	int ka_show_table;
	int ka_show_length;
};



static void crack(const struct crack_args *a) {
	struct cracker ck;
	struct vigenere vig;


	ck_init(&ck, a->str, VIG_CHARSET_UPPER);

	if (a->klen != 0)
		ck_set_length(&ck, a->klen);

	if (a->ka_minlen != 0)
		ck.ka_minlen = a->ka_minlen;

	ck_crack(&ck);

	if (a->ka_show_table)
		crack_ka_show_table(&ck);


	if (a->ka_show_length)
		crack_ka_show_length(&ck);


	printf("Found key: %s\n", ck.key);

	vig_init(&vig, a->str);
	vig_add_charset(&vig, VIG_CHARSET_UPPER);
	vig_add_charset(&vig, VIG_CHARSET_LOWER);
	vig_decrypt(&vig, ck.key);
	vig_fini(&vig);

	ck_fini(&ck);
}



static void simple_action(struct fs_ctx *s, const char *key, enum action act) {
	struct vigenere vig;

	vig_init(&vig, s);
	vig_add_charset(&vig, VIG_CHARSET_UPPER);
	vig_add_charset(&vig, VIG_CHARSET_LOWER);

	if (act == ACTION_ENCRYPT)
		vig_encrypt(&vig, key);
	else if (act == ACTION_DECRYPT)
		vig_decrypt(&vig, key);
	else
		custom_error("Dafuq? simple_action called with an unknown action");

	vig_fini(&vig);
}



/*
 * Load a file into a buffer allocated with malloc. Must be freed by caller.
 * The filename may be "-" to read from stdin.
 */
static char *read_file(const char *filename) {
	FILE *fp = stdin;
	ARRAY_DECL(char, buffer);
	size_t nr;
	int err;
	char nulchar = '\0';

	if (strcmp(filename, "-") != 0) {
		fp = fopen(filename, "r");
		if (fp == NULL)
			system_error(filename);
	}

	ARRAY_ALLOC(buffer, 8);

	do {
		/* Grow the buffer if needed. */
		if (buffer_size >= buffer_mem)
			ARRAY_GROW(buffer);

		nr = fread(buffer + buffer_size, 1, buffer_mem - buffer_size, fp);
		buffer_size += nr;
	} while (nr > 0);

	/* TODO: It sucks a bit to need a variable nulchar for this. */
	ARRAY_APPEND(buffer, nulchar);

	if (ferror(fp))
		system_error("fread");

	if (strcmp(filename, "-") != 0) {
		err = fclose(fp);
		if (err == -1)
			system_error("fclose");
	}

	return buffer;
}



/*
 * Write some data to a file. May be "-" to write to stdout.
 */
static void write_file(const char *filename, const char *data) {
	FILE *fp = stdout;
	size_t len;
	size_t nw;
	int err;


	if (strcmp(filename, "-") != 0) {
		fp = fopen(filename, "w");
		if (fp == NULL)
			system_error(filename);
	}

	len = strlen(data);

	nw = fwrite(data, 1, len, fp);
	if (ferror(fp))
		system_error("fwrite");

	if (nw != len)
		custom_error("Dafuq? Didn't write everything we should.");

	if (strcmp(filename, "-") != 0) {
		err = fclose(fp);
		if (err == -1)
			system_error("fclose");
	}
}



int main(int argc, char **argv) {
	struct goh_state st;
	int opt;
	struct fs_ctx s;
	enum action action = ACTION_CRACK;
	char *key = NULL;
	const char *filenamein = "-";
	const char *filenameout = "-";
	char *text = NULL;
	struct crack_args cka;

	memset(&cka, 0, sizeof(cka));

	/* Parse the options. */
	goh_init(&st, opt_desc, ARRAY_LENGTH(opt_desc), argc, argv, 1);
	st.usagehelp = "[options]\n";

	while ((opt = goh_nextoption(&st)) >= 0) {
		switch (opt) {
		case 'i':
			filenamein = st.argval;
			break;

		case 'o':
			filenameout = st.argval;
			break;

		case 'e':
			action = ACTION_ENCRYPT;
			break;

		case 'd':
			action = ACTION_DECRYPT;
			break;

		case 'k':
			key = st.argval;
			break;

		case 'l':
			cka.klen = atoi(st.argval);
			break;

		case OPT_KASISKI_MIN_LENGTH:
			cka.ka_minlen = atoi(st.argval);
			break;

		case OPT_SHOW_KASISKI_TABLE:
			cka.ka_show_table = 1;
			break;

		case OPT_SHOW_KASISKI_LENGTH:
			cka.ka_show_length = 1;
			break;

		default:
			custom_error("Unrecognized option (shouldn't happen)");
			break;
		}
	}


	/* Common command line mistake. */
	if (st.argidx != argc)
		custom_error("Useless argument %s", argv[st.argidx]);

	goh_fini(&st);

	/* Check for some invalid options combinations. */
	if (action != ACTION_CRACK && key == NULL)
		custom_error("Encryption and decryption take a --key");

	if (action == ACTION_CRACK && key != NULL)
		custom_error("--key need either --encrypt or --decrypt");

	if (cka.klen > 0 && key != NULL)
		custom_warn("Unnecessary key length option with an actual key");

	if (cka.klen > 0 && key != NULL && cka.klen != strlen(key))
		custom_error("Key length option doesn't match "
		             "the length of the key");

	if (cka.ka_minlen > 0 && action != ACTION_CRACK)
		custom_error("--kasiski-min-length can only be used in "
		             "cracking mode");

	if (cka.ka_minlen > 0 && cka.klen > 0)
		custom_warn("Useless option --kasiski-min-length when the key "
		            "length is given");

	if (cka.ka_show_table != 0 && action != ACTION_CRACK)
		custom_error("--show-kasiski-table can only be used in "
		             "cracking mode");

	if (cka.ka_show_table != 0 && cka.klen > 0)
		custom_warn("Option --show-kasiski-table ignored when a key "
		            "length is given");

	if (cka.ka_show_length != 0 && action != ACTION_CRACK)
		custom_error("--show-kasiski-length can only be used in "
		             "cracking mode");

	if (cka.ka_show_length != 0 && cka.klen > 0)
		custom_warn("Option --show-kasiski-length ignored when a key "
		            "length is given");


	/* Start to do the job. */
	text = read_file(filenamein);
	fs_init(&s, text, FS_CHARSET_ALPHA);
	cka.str = &s;

	if (action == ACTION_CRACK)
		crack(&cka);
	else
		simple_action(&s, key, action);

	write_file(filenameout, text);

	fs_fini(&s);
	free(text);


	return EXIT_SUCCESS;
}
