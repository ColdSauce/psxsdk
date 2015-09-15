#ifndef _SPASM_H
#define _SPASM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "gtext.h"
#include "error.h"
#include "codegen.h"
#include "parser.h"
#include "opcode.h"
#include "spasm.h"
#include "eval.h"

FILE *spasm_fopen(const char *path, const char *mode);

#endif
