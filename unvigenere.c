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
		"Show the score table for the kasiski method."}
};




static void crack(struct fs_ctx *s, size_t len, size_t ka_minlen,
                  int ka_show_table) {

	struct cracker ck;
	struct vigenere vig;


	ck_init(&ck, s, VIG_CHARSET_UPPER);

	if (len != 0)
		ck_set_length(&ck, len);

	if (ka_minlen != 0)
		ck.ka_minlen = ka_minlen;

	ck_crack(&ck);

	if (ka_show_table) {
		size_t i;
		size_t nlen;

		printf("Kasiski score table:\n");

		nlen = strlen(s->norm);
		for (i = 0; i < nlen; i++)
			printf("%lu: %lu\n", i, ck.ka.score[i]);
	}


	printf("Found key: %s\n", ck.key);

	vig_init(&vig, s);
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
	size_t klen = 0;
	const char *filenamein = "-";
	const char *filenameout = "-";
	char *text = NULL;
	size_t ka_minlen = 0;
	int ka_show_table = 0;


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
			klen = atoi(st.argval);
			break;

		case OPT_KASISKI_MIN_LENGTH:
			ka_minlen = atoi(st.argval);
			break;

		case OPT_SHOW_KASISKI_TABLE:
			ka_show_table = 1;
			break;

		default:
			custom_error("Unrecognized option (shouldn't happen)");
			break;
		}
	}


	/* Common command line mistakes. */
	if (st.argidx != argc)
		custom_error("Useless argument %s", argv[st.argidx]);

	goh_fini(&st);

	if (action != ACTION_CRACK && key == NULL)
		custom_error("Encryption and decryption take a --key.");

	if (action == ACTION_CRACK && key != NULL)
		custom_error("--key need either --encrypt or --decrypt");

	if (klen > 0 && key != NULL)
		custom_warn("Unnecessary key length option with an actual key");

	if (klen > 0 && key != NULL && klen != strlen(key))
		custom_error("Key length option doesn't match "
		             "the length of the key.");


	/* Start to do the job. */
	text = read_file(filenamein);
	fs_init(&s, text, FS_CHARSET_ALPHA);

	if (action == ACTION_CRACK)
		crack(&s, klen, ka_minlen, ka_show_table);
	else
		simple_action(&s, key, action);

	write_file(filenameout, text);

	fs_fini(&s);
	free(text);


	return EXIT_SUCCESS;
}
