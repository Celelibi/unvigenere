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

#include "charset.h"
#include "filtered_string.h"


/*
 * Encrypt the text str using the key key.
 */
void vig_encrypt(struct fs_ctx *str, const char *key);

/*
 * Encrypt the text str using the key key.
 */
void vig_decrypt(struct fs_ctx *str, const char *key);

#endif
