#ifndef REPORT_ERROR_H
#define REPORT_ERROR_H

#ifndef XLTS_H_INCLUDED
#define XLTS_H_INCLUDED
#include "xlts.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Report using xlts as location reference */
void re_error(struct xlts *where, const char *fmt, ...);

/* Report using the last matched xlts (tkr->xmatch_) as location reference */
void re_error_tkr(struct tkr_tokenizer *tkr, const char *fmt, ...);

/* Report using explicit file, line, column location reference */
void re_error_flc(const char *filename, int line, int col, const char *fmt, ...);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* REPORT_ERROR_H */
