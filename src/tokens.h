#ifndef TOKENS_H
#define TOKENS_H

#ifndef STDINT_H_INCLUDED
#define STDINT_H_INCLUDED
#include <stdint.h>
#endif

#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED
#include "tokenizer.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

  /* ENUM_SCANNER_DEFS -- enumerates the entire scanner by invoking the xx() macro for
   * each token minor_type.
   * xx(regex_string, minor_type)
   * xy(regex_string, minor_type, major_type)
   * xz(major_type)
   * regex_string: is the regular expression that describes the token
   * minor_type: minor enumeration value for the token. These match 1:1 on the regular expression.
   * major_type: major enumeration value for the token. These are in terms a parser would use.
   * An example for a major_type would be TOK_INTEGER, an example for a minor type would be
   * TOK_OCTAL_INTEGER. Most tokens have the same major and minor types, for these the xx macro
   * is used.
   * The unique major types are themselves enumerated in xz(major_type)
   */
#define ENUM_SCANNER_DEFS \
xz(TOK_NO_MATCH) \
xx("!", TOK_BANG) \
xx("~", TOK_TILDE) \
xx("%", TOK_PERCENT) \
xx("%=", TOK_PERCENT_EQUALS) \
xx("&", TOK_AMPERSAND) \
xx("&&", TOK_AMPERSAND_AMPERSAND) \
xx("&=", TOK_AMPERSAND_EQUALS) \
xx("\\(", TOK_PAR_OPEN) \
xx("\\)", TOK_PAR_CLOSE) \
xx("\\*", TOK_ASTERISK) \
xx("\\*=", TOK_ASTERISK_EQUALS) \
xx("\\+", TOK_PLUS) \
xx("\\+\\+", TOK_PLUS_PLUS) \
xx("\\,", TOK_COMMA) \
xx("\\-", TOK_MINUS) \
xx("\\-\\-", TOK_MINUS_MINUS) \
xx("\\-=", TOK_MINUS_EQUALS) \
xx("\\.", TOK_DOT) \
xx("\\.\\.\\.", TOK_DOT_DOT_DOT) \
xx("/", TOK_SLASH) \
xx("/=", TOK_SLASH_EQUALS) \
xx("\\:", TOK_COLON) \
xx(";", TOK_SEMICOLON) \
xx("<", TOK_LT) \
xx("<<", TOK_LT_LT) \
xx("<<=", TOK_LT_LT_EQUALS) \
xx("<=", TOK_LT_EQUALS) \
xx("=", TOK_EQUALS) \
xx("==", TOK_EQUALS_EQUALS) \
xx("!=", TOK_BANG_EQUALS) \
xx(">", TOK_GT) \
xx(">=", TOK_GT_EQUALS) \
xx(">>", TOK_GT_GT) \
xx(">>=", TOK_GT_GT_EQUALS) \
xx("\\->", TOK_ARROW) \
xx("\\[", TOK_SQBRACE_OPEN) \
xx("\\]", TOK_SQBRACE_CLOSE) \
xx("\\^", TOK_CARET) \
xx("\\^=", TOK_CARET_EQUALS) \
xx("\\{", TOK_CUBRACE_OPEN) \
xx("\\}", TOK_CUBRACE_CLOSE) \
xx("\\|", TOK_BAR) \
xx("\\|=", TOK_BAR_EQUALS) \
xx("\\|\\|", TOK_BAR_BAR) \
xx("\\?", TOK_QUESTION_MARK) \
/* keywords go here */ \
\
xz(TOK_INTEGER_LIT) \
xy("[1-9][0-9]*"       "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_DECIMAL_INTEGER_LIT, TOK_INTEGER_LIT) \
xy("0[0-7]*"           "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_OCTAL_INTEGER_LIT, TOK_INTEGER_LIT) \
xy("0[xX][0-9a-fA-F]+" "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_HEX_INTEGER_LIT, TOK_INTEGER_LIT) \
xy("0[bB][01]+"        "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_BINARY_INTEGER_LIT, TOK_INTEGER_LIT)\
\
xz(TOK_FLOAT_LIT) \
xy("([0-9]+\\.)|([0-9]*\\.[0-9]+)" "([eE][\\+\\-]?[0-9]+)?" "[flFL]?", TOK_FRACTIONAL_FLOAT_LIT, TOK_FLOAT_LIT) \
xy("[0-9]+"                         "[eE][\\+\\-]?[0-9]+"   "[flFL]?", TOK_WHOLE_FLOAT_LIT, TOK_FLOAT_LIT) \
\
xx("L?\"(\\\\.|[^\"])*\"", TOK_STRING_LIT) \
xx("L?\'(\\\\.|[^\'])*\'", TOK_CHAR_LIT) \
\
xx("[a-zA-Z_][0-9a-zA-Z_]*", TOK_IDENT) \
xx("\\$[0-9a-zA-Z_\\-]*", TOK_SPECIAL_IDENT) /* special identifier with - characters allowed, leads with $ sign */ \
\
xz(TOK_WHITESPACE) \
xy("/\\*([^\\*]|[\\r\\n]|(\\*+([^\\*/]|[\\r\\n])))*\\*/", TOK_C_STYLE_COMMENT, TOK_WHITESPACE) \
xy("//.*", TOK_CPP_STYLE_COMMENT, TOK_WHITESPACE) \
xy("\\n", TOK_NEWLINE, TOK_WHITESPACE) \
xy("[ \\x9\\xb\\xc\\xd]+", TOK_WHITESPACE_CHAR, TOK_WHITESPACE)

#define ENUM_SCANNER_PRODUCTION_DEFS \
xz(TOK_NO_MATCH) \
xx("!", TOK_BANG) \
xx("~", TOK_TILDE) \
xx("%", TOK_PERCENT) \
xx("%=", TOK_PERCENT_EQUALS) \
xx("&", TOK_AMPERSAND) \
xx("&&", TOK_AMPERSAND_AMPERSAND) \
xx("&=", TOK_AMPERSAND_EQUALS) \
xx("\\(", TOK_PAR_OPEN) \
xx("\\)", TOK_PAR_CLOSE) \
xx("\\*", TOK_ASTERISK) \
xx("\\*=", TOK_ASTERISK_EQUALS) \
xx("\\+", TOK_PLUS) \
xx("\\+\\+", TOK_PLUS_PLUS) \
xx("\\,", TOK_COMMA) \
xx("\\-", TOK_MINUS) \
xx("\\-\\-", TOK_MINUS_MINUS) \
xx("\\-=", TOK_MINUS_EQUALS) \
xx("\\.", TOK_DOT) \
xx("\\.\\.\\.", TOK_DOT_DOT_DOT) \
xx("/", TOK_SLASH) \
xx("/=", TOK_SLASH_EQUALS) \
xx("\\:", TOK_COLON) \
xx(";", TOK_SEMICOLON) \
xx("<", TOK_LT) \
xx("<<", TOK_LT_LT) \
xx("<<=", TOK_LT_LT_EQUALS) \
xx("<=", TOK_LT_EQUALS) \
xx("=", TOK_EQUALS) \
xx("==", TOK_EQUALS_EQUALS) \
xx("!=", TOK_BANG_EQUALS) \
xx(">", TOK_GT) \
xx(">=", TOK_GT_EQUALS) \
xx(">>", TOK_GT_GT) \
xx(">>=", TOK_GT_GT_EQUALS) \
xx("\\->", TOK_ARROW) \
xx("\\[", TOK_SQBRACE_OPEN) \
xx("\\]", TOK_SQBRACE_CLOSE) \
xx("\\^", TOK_CARET) \
xx("\\^=", TOK_CARET_EQUALS) \
xx("\\{", TOK_CUBRACE_OPEN) \
xx("\\}", TOK_CUBRACE_CLOSE) \
xx("\\|", TOK_BAR) \
xx("\\|=", TOK_BAR_EQUALS) \
xx("\\|\\|", TOK_BAR_BAR) \
xx("\\?", TOK_QUESTION_MARK) \
/* keywords go here */ \
\
xz(TOK_INTEGER_LIT) \
xy("[1-9][0-9]*"       "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_DECIMAL_INTEGER_LIT, TOK_INTEGER_LIT) \
xy("0[0-7]*"           "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_OCTAL_INTEGER_LIT, TOK_INTEGER_LIT) \
xy("0[xX][0-9a-fA-F]+" "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_HEX_INTEGER_LIT, TOK_INTEGER_LIT) \
xy("0[bB][01]+"        "(((U|u)(L|l|LL|ll)?)|((L|l|LL|ll)(U|u)?))?", TOK_BINARY_INTEGER_LIT, TOK_INTEGER_LIT)\
\
xz(TOK_FLOAT_LIT) \
xy("([0-9]+\\.)|([0-9]*\\.[0-9]+)" "([eE][\\+\\-]?[0-9]+)?" "[flFL]?", TOK_FRACTIONAL_FLOAT_LIT, TOK_FLOAT_LIT) \
xy("[0-9]+"                         "[eE][\\+\\-]?[0-9]+"   "[flFL]?", TOK_WHOLE_FLOAT_LIT, TOK_FLOAT_LIT) \
\
xx("L?\"(\\\\.|[^\"])*\"", TOK_STRING_LIT) \
xx("L?\'(\\\\.|[^\'])*\'", TOK_CHAR_LIT) \
\
xx("[a-zA-Z_][0-9a-zA-Z_]*", TOK_IDENT) \
xx("[a-zA-Z_\\-][0-9a-zA-Z_\\-]*", TOK_IDENT) /* identifier with - characters allowed */ \
xx("\\$[0-9a-zA-Z_\\-]*", TOK_SPECIAL_IDENT) /* special identifier with - characters allowed, leads with $ sign */ \
\
xz(TOK_WHITESPACE) \
xy("/\\*([^\\*]|[\\r\\n]|(\\*+([^\\*/]|[\\r\\n])))*\\*/", TOK_C_STYLE_COMMENT, TOK_WHITESPACE) \
xy("//.*", TOK_CPP_STYLE_COMMENT, TOK_WHITESPACE) \
xy("\\n", TOK_NEWLINE, TOK_WHITESPACE) \
xy("[ \\x9\\xb\\xc\\xd]+", TOK_WHITESPACE_CHAR, TOK_WHITESPACE)


typedef enum token_type {
#define xx(regex, minor) minor,
#define xy(regex, minor, major) minor,
#define xz(major) major,
  ENUM_SCANNER_DEFS
#undef xx
#undef xy
#undef xz
} token_type_t;

int tok_init(void);
void tok_cleanup(void);

/* Initializes a TKR tokenizer with the symbols for tokens */
void tok_init_tkr_tokenizer(struct tkr_tokenizer *tkr);

/* Switch to C identifiers (eg. identifiers with no '-' characters) */
void tok_switch_to_c_idents(struct tkr_tokenizer *tkr);

/* Switch to non-terminal identifiers (eg. identifiers with '-' permitted.) */
void tok_switch_to_nonterminal_idents(struct tkr_tokenizer *tkr);

const char *tok_token_type_to_str(token_type_t tkt);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TOKENS_H */
