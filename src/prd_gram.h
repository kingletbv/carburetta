#ifndef PRD_GRAM_H
#define PRD_GRAM_H

#ifndef TOKENS_H_INCLUDED
#define TOKENS_H_INCLUDED
#include "tokens.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

int prd_init(void);
void prd_cleanup(void);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* PRD_GRAM_H */