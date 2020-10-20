# Cinder - Parser Generator

Cinder is a parser generator for C and C++. It aims to make parsing of smaller languages trivial and larger languages doable.

## Getting Started

```c
#include <stdio.h>

%%

grammar: expression { printf("Evaluation: %d\n", $0); }

factor: NUMBER;
factor: PAR_OPEN expression PAR_CLOSE;

term: factor;
term: term MULTIPLY factor;
term: term DIVIDE factor;

expression: term { $$ = $0; }
expression: expression PLUS term { $$ = $0 + $2; }
expression: expression MINUS term { $$ = $0 - $2; }

%%

int main(int argc, char **argv) {
  printf("Enter an expression: ");
  static char s[300];
  while (fgets(s, sizeof(s), stdin)) {
    
  }
}

```

## Usage

```shell
$ cinder inputfile.cnd --c parser.c --h
```

Will read the input grammar in `inputfile.cnd` and generate the C file `parser.c` containing the parser code, and a header file `parser.h` containing the declarations
necessary for calling the parser from another source file.

