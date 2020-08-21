#ifndef STDIO_H_INCLUDED
#define STDIO_H_INCLUDED
#include <stdio.h>
#endif

#ifndef STDLIB_H_INCLUDED
#define STDLIB_H_INCLUDED
#include <stdlib.h>
#endif

#ifndef STRING_H_INCLUDED
#define STRING_H_INCLUDED
#include <string.h>
#endif

#ifndef CTYPE_H_INCLUDED
#define CTYPE_H_INCLUDED
#include <ctype.h>
#endif

#ifndef HELPERS_H_INCLUDED
#define HELPERS_H_INCLUDED
#include "helpers.h"
#endif

#ifndef KLT_LOGGER_H_INCLUDED
#define KLT_LOGGER_H_INCLUDED
#define KLT_LOG_MODULE "cinder"
#include "klt_logger.h"
#endif

#ifndef LOG_FUNCTION_H_INCLUDED
#define LOG_FUNCTION_H_INCLUDED
#include "log_function.h"
#endif

#ifndef SCANNER_H_INCLUDED
#define SCANNER_H_INCLUDED
#include "scanner.h"
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifndef LINE_DEFS_H_INCLUDED
#define LINE_DEFS_H_INCLUDED
#include "line_defs.h"
#endif


int main(int argc, char **argv) {
  klt_logger_init();
  klt_log_set_function("/", log_function, NULL);

  LOG("We've started..\n");

  prd_init();

  struct tkr_tokenizer tkr;
  ldl_init_tokenizer(&tkr);

  struct ld_line ldl;
  ld_line_init(&ldl);

  const char *input_filename = "test_input.cnd";
  FILE *fp = fopen(input_filename, "rb");
  if (!fp) {
    LOGERROR("Failed to open file \"%s\"\n", input_filename);
    return EXIT_FAILURE;
  }

  size_t num_bytes_read;
  static char buf[2400];
  do {
    num_bytes_read = fread(buf, sizeof(*buf), sizeof(buf) / sizeof(*buf), fp);
    int r;
    
    r = tkr_tokenizer_input(&tkr, buf, num_bytes_read, !num_bytes_read);
    while ((r != TKR_END_OF_INPUT) && (r != TKR_FEED_ME)) {

      if (r == TKR_SYNTAX_ERROR) {
        if (isprint(tkr.match_[0])) {
          LOGERROR("%s(%d): Character \"%s\" not expected at column %d\n", tkr.filename_, tkr.best_match_line_, tkr.match_, tkr.best_match_col_);
        }
      }
      else if (r == TKR_INTERNAL_ERROR) {

        num_bytes_read = 0;
        break;
      }

      if (r == TKR_MATCH) {
        printf("Match %s:\n", ld_line_type_to_str(tkr.best_match_action_));
        printf("%s", tkr.match_);
        if (tkr.match_[tkr.token_size_ - 1] != '\n') {
          /* Last line of input has no trailing newline */
          printf("\n");
        }
      }

      r = tkr_tokenizer_input(&tkr, buf, num_bytes_read, !num_bytes_read);
    }
  } while (num_bytes_read);

  tkr_tokenizer_cleanup(&tkr);
  LOG("We've finished\n");

  return EXIT_SUCCESS;
}
