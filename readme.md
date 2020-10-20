# Cinder - Parser Generator

Cinder is a parser generator for C and C++. It aims to make parsing of smaller languages trivial and larger languages doable.

## Getting Started

```c
#include <stdio.h>

%%
%prefix calc_
%nt grammar expr term factor value
%nt_type grammar expr term factor value: int
%token PLUS MINUS ASTERISK SLASH TILDE BANG PAR_OPEN PAR_CLOSE INTEGER 
%token_type int
%token_action $$ = sym_value;
%params int sym_value, int *final_result

grammar: expr { printf("Outcome: %d\n", $0); *final_result = $0; }

expr: term                      { $$ = $0; }
expr: expr PLUS term            { $$ = $0 + $2; }
expr: expr MINUS term           { $$ = $0 - $2; }

term: factor                    { $$ = $0; }
term: term ASTERISK factor      { $$ = $0 * $2; }
term: term SLASH factor         { $$ = $0 / $2; }

factor: value                   { $$ = $0; }
factor: TILDE factor            { $$ = ~$1; }
factor: BANG factor             { $$ = !$1; }
factor: MINUS factor            { $$ = -$1; }
factor: PAR_OPEN expr PAR_CLOSE { $$ = $1; }

value: INTEGER                  { $$ = $0; }

%%

int main(int argc, char **argv) {
  int final_result = 0;
  struct calc_stack stack;
  calc_stack_init(&stack);
  static char s[300];
  fprintf(stderr, "Enter an expression (-1 to terminate):\n");
  do {
    if (!fgets(s, sizeof(s), stdin)) {
      calc_stack_cleanup(&stack);
      return -1;
    }
    calc_stack_reset(&stack);
    const char *p = s;
    int pass_error = 0;
    for (p = s; (p < (s+sizeof(s))) && *p; ++p) {
      int sym;
      int sym_value = 0;
      int have_error = 0;
      const char *sym_start = p;
      switch (*p) {
        case '+': sym = CALC_PLUS; break;
        case '-': sym = CALC_MINUS; break;
        case '*': sym = CALC_ASTERISK; break;
        case '/': sym = CALC_SLASH; break;
        case '~': sym = CALC_TILDE; break;
        case '!': sym = CALC_BANG; break;
        case '(': sym = CALC_PAR_OPEN; break;
        case ')': sym = CALC_PAR_CLOSE; break;
        case ' ':
        case '\n':
          continue; /* don't process whitespace as token */
        default:
          if ((*p >= '0') && (*p <= '9')) {
            sym = CALC_INTEGER;
            do {
              sym_value = sym_value * 10 + (*p - '0');
              p++;
            
            } while (((p < (s + sizeof(s))) && *p) && ((*p >= '0') && (*p <= '9')));
            /* p is on the first char beyond the digits, we want it on the last digit (p++ in for(;;) is coming) */
            p--;
            break;
          }
          else {
            have_error = 1;
            if (!pass_error) {
              fprintf(stderr, "Syntax error:\n%s%*s\n", s, 1 + (int)(p - s), "^");
            }
            pass_error = 1;
            break;
          }
      }
      if (!have_error) {
        pass_error = 0;
        switch (calc_parse(&stack, sym, sym_value, &final_result)) {
          case -2: fprintf(stderr, "Internal error\n");
                   return - 1;
          case -1: fprintf(stderr, "Syntax error:\n%s%*s\n", s, 1 + (int)(sym_start - s), "^");
                   return - 1;
          case 0: /* finished prior to grammar completion?? */
                  fprintf(stderr, "Unexpected grammar finish:\n%s%*s\n", s, 1 + (int)(sym_start - s), "^");
          case 1: /* get next symbol */
                  break;
        }
      }
    }
    calc_parse(&stack, CALC_INPUT_END, 0, &final_result);
  } while (final_result != -1);
  calc_stack_cleanup(&stack);

  return final_result;
}
```

## Usage

```shell
$ cinder inputfile.cnd --c parser.c --h
```

Will read the input grammar in `inputfile.cnd` and generate the C file `parser.c` containing the parser code, and a header file `parser.h` containing the declarations
necessary for calling the parser from another source file.

