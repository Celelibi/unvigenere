#ifndef CHARSET_H__
#define CHARSET_H__

/* This module handle the set of supported characters and support equivalent
 * classes of characters.
 * Every call to cs_add add a new set of characters equivalent to the previous
 * ones. For instance adding "abcd" then "ABC" say that 'a' is equivalent to 'A'
 * and so on. And 'd' do not has an equivalent character. */


#include "array.h"


/* Just a few usual characters sets that may be used. */
#define CHARSET_UPPER "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define CHARSET_LOWER "abcdefghijklmnopqrstuvwxyz"
#define CHARSET_ALPHA (CHARSET_UPPER CHARSET_LOWER)
#define CHARSET_NUMER "0123456789"
#define CHARSET_HEXUP (CHARSET_NUMER "ABCDEF")
#define CHARSET_HEXLO (CHARSET_NUMER "abcdef")
#define CHARSET_HEX (CHARSET_NUMER "ABCDEF" "abcdef")


struct charset {
	ARRAY_DECL(char *, chars);
};



/* Initialize an empty charset. */
void cs_init(struct charset *cs);

/* Deinitialize a charset. */
void cs_fini(struct charset *cs);

/* Add a list of chars to the charset. */
void cs_add(struct charset *cs, const char *str);

/* Ask whether a character belong to the charset. Return 1 if yes, 0 if no. */
int cs_belong(const struct charset *cs, char c);

/* Ask whether two characters are equivalents. */
int cs_equiv(const struct charset *cs, char c1, char c2);

/* Retrive the normalized version of a character. That may be any character
 * give, it is guaranteed to be always the same. Return c if the character do
 * not belong to the charset. */
char cs_norm(const struct charset *cs, char c);

/* Equivalent to strpbrk with a struct charset as set of accepted characters.
 * /!\ The arguments are inverted w.r.t strpbrk to keep consistent with the
 * other functions of this module. */
char *cs_strpbrk(const struct charset *cs, const char *str);

#endif
