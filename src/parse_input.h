#ifndef PARSE_INPUT_H
#define PARSE_INPUT_H

#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int pi_parse_input(FILE *fp, const char *input_filename, struct carburetta_context *cc, struct prd_grammar *prdg);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PARSE_INPUT_H */
