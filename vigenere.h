#ifndef VIGERERE_H__
#define VIGERERE_H__

/*
 * Encode or decode a string using the Vigenere cypher.
 * It can be configured with several charsets so as to keep letter case for
 * instance. When a char is found to be within a charset, it will be encoded /
 * decoded inside this charset.
 * The key may be given in any charset.
 * For instance, two charsets "ABCDEFGHIJKLMNOPQRSTUVWXYZ" and
 * "abcdefghijklmnopqrstuvwxyz" for a key "AbC" encode "Hello" into "Igomq".
 * A character that do not belong to a charset is not modified.
 */


#include <sys/types.h>

#include "filtered_string.h"


#define VIG_CHARSET_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define VIG_CHARSET_LOWER "abcdefghijklmnopqrstuvwxyz"



struct vigenere {
	struct fs_ctx *str;

	const char **charsets;
	size_t charsets_size;
	size_t charsets_mem;

	size_t charset_len;
};


/*
 * Initialize a struct vigenere with a given string.
 * The string is not copied and must remain available.
 * All the caracters of the charset of str must be added as charsets of vig.
 */
void vig_init(struct vigenere *vig, struct fs_ctx *str);

/*
 * Deinitialize a struct vigenere.
 */
void vig_fini(struct vigenere *vig);

/*
 * Add a charset to the encryptor / decryptor.
 * The charset is not copied and most remain available.
 * All the charsets added must match the charset of vig->str.
 */
void vig_add_charset(struct vigenere *vig, const char *charset);

/*
 * Encrypt the text str using the key key.
 */
void vig_encrypt(struct vigenere *vig, const char *key);

/*
 * Encrypt the text str using the key key.
 */
void vig_decrypt(struct vigenere *vig, const char *key);

#endif
