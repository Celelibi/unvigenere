#include <stdio.h>
#include <stdlib.h>

#include "misc.h"


void system_error(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}
