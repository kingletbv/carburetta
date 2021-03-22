# Carburetta - Parser Generator

Carburetta is a parser generator for C and C++. It aims to make parsing of smaller languages trivial and larger languages doable.

Please note that this project is _not accepting pull requests_, both due to the intricacies of the code making isolated changes impractical, and the impracticality of determining contribution ownership.

To learn more about Carburetta, please go to its website at [https://carburetta.com/](https://carburetta.com/)

## Getting Started

```c
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

%scanner%
%prefix calc_

: [\ \n]+; /* skip whitespace */ 

PLUS: \+;
MINUS: \-;
ASTERISK: \*;
SLASH: /;
TILDE: ~;
BANG: !;
PAR_OPEN: \(;
PAR_CLOSE: \);
INTEGER: [0-9]+ {
  $$ = atoi($text);
}

%token PLUS MINUS ASTERISK SLASH TILDE BANG PAR_OPEN PAR_CLOSE INTEGER
%nt grammar expr term factor value


%grammar%

%type grammar expr term factor value: int

%token_type int
%constructor $$ = 0;
%params int *final_result

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
  struct calc_stack stack;
  calc_stack_init(&stack);
  
  int final_result;
  static char s[300];

  fprintf(stderr, "Enter an expression (-1 to terminate):\n");

  do {
    if (!fgets(s, sizeof(s), stdin)) {
      break;
    }
    calc_stack_reset(&stack);
    calc_scan(&stack, s, strlen(s), 1, &final_result);
  } while (final_result != -1);
  calc_stack_cleanup(&stack);

  return final_result;
}

```

## Usage

```shell
$ carburetta inputfile.cnd --c parser.c --h
```

Will read the input grammar in `inputfile.cnd` and generate the C file `parser.c` containing the parser code, and a header file `parser.h` containing the declarations
necessary for calling the parser from another source file.

