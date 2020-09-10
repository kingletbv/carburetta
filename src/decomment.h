#ifndef DECOMMENT_H
#define DECOMMENT_H

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Removes C-Style and C++ Style comments from an xlts string, useful when the tokenizer that next parses the string does not support comments. */
int dct_init();
void dct_cleanup();

int dct_decomment(struct xlts *val);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DECOMMENT_H */
