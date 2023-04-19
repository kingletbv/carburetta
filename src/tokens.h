/* Copyright 2020-2023 Kinglet B.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TOKENS_H
#define TOKENS_H

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
xx("L?\"(\\\\.|[^\"\\\\])*\"", TOK_STRING_LIT) \
xx("L?\'(\\\\.|[^\'\\\\])*\'", TOK_CHAR_LIT) \
\
xx("[a-zA-Z_][0-9a-zA-Z_]*", TOK_IDENT) \
xz(TOK_SPECIAL_IDENT) \
xy("\\$[0-9a-zA-Z_]*", TOK_SPECIAL_IDENT_STR, TOK_SPECIAL_IDENT) /* special identifier with - characters allowed, leads with $ sign */ \
xy("\\$\\$", TOK_SPECIAL_IDENT_DST, TOK_SPECIAL_IDENT) /* special identifier $$ (can only be 1) */ \
\
xz(TOK_WHITESPACE) \
xy("/\\*([^\\*]|[\\r\\n]|(\\*+([^\\*/]|[\\r\\n])))*\\*/", TOK_C_STYLE_COMMENT, TOK_WHITESPACE) \
xy("//.*", TOK_CPP_STYLE_COMMENT, TOK_WHITESPACE) \
xy("\\n", TOK_NEWLINE, TOK_WHITESPACE) \
xy("[ \\x9\\xb\\xc\\xd]+", TOK_WHITESPACE_CHAR, TOK_WHITESPACE) \
xz(TOK_RESERVED_REGEX_CHAR) \
xz(TOK_DOLLAR) \
xz(TOK_CHAR) \
xz(TOK_CATEGORY) \
xz(TOK_REGULAR_CATEGORY) \
xz(TOK_INVERTED_CATEGORY) \
xz(TOK_ESC_A) \
xz(TOK_ESC_B) \
xz(TOK_ESC_F) \
xz(TOK_ESC_N) \
xz(TOK_ESC_R) \
xz(TOK_ESC_T) \
xz(TOK_ESC_V) \
xz(TOK_ESC_CHAR) \
xz(TOK_ESC_HEX1) \
xz(TOK_ESC_HEX2) \
xz(TOK_ESC_OCT1) \
xz(TOK_ESC_OCT2) \
xz(TOK_ESC_OCT3) \
xz(TOK_ESC_UNI1) \
xz(TOK_ESC_UNI2) \
xz(TOK_ESC_INVALID_ESCAPE) \
xz(TOK_ESC_START_OF_INPUT_ESC) \
xz(TOK_ESC_END_OF_INPUT_ESC)


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
xx("L?\"(\\\\.|[^\"\\\\])*\"", TOK_STRING_LIT) \
xx("L?\'(\\\\.|[^\'\\\\])*\'", TOK_CHAR_LIT) \
\
xx("[a-zA-Z_][0-9a-zA-Z_]*", TOK_IDENT) \
xx("[a-zA-Z_\\-][0-9a-zA-Z_\\-]*", TOK_IDENT) /* identifier with - characters allowed */ \
xz(TOK_SPECIAL_IDENT) \
xy("\\$[0-9a-zA-Z_]*", TOK_SPECIAL_IDENT_STR, TOK_SPECIAL_IDENT) /* special identifier with - characters allowed, leads with $ sign */ \
xy("\\$\\$", TOK_SPECIAL_IDENT_DST, TOK_SPECIAL_IDENT) /* special identifier $$ (can only be 1) */ \
\
xz(TOK_WHITESPACE) \
xy("/\\*([^\\*]|[\\r\\n]|(\\*+([^\\*/]|[\\r\\n])))*\\*/", TOK_C_STYLE_COMMENT, TOK_WHITESPACE) \
xy("//.*", TOK_CPP_STYLE_COMMENT, TOK_WHITESPACE) \
xy("\\n", TOK_NEWLINE, TOK_WHITESPACE) \
xy("[ \\x9\\xb\\xc\\xd]+", TOK_WHITESPACE_CHAR, TOK_WHITESPACE)

#define ENUM_SCANNER_REGEX_DEFS \
xx("\\(", TOK_PAR_OPEN) \
xx("\\)", TOK_PAR_CLOSE) \
xx("\\[", TOK_SQBRACE_OPEN) \
xx("\\]", TOK_SQBRACE_CLOSE) \
xx("\\.", TOK_DOT) \
xx("\\$", TOK_DOLLAR) \
xx("\\^", TOK_CARET) \
xx("\\-", TOK_MINUS) \
xx("\\|", TOK_BAR) \
xx("\\*", TOK_ASTERISK) \
xx("\\+", TOK_PLUS) \
xx("\\?", TOK_QUESTION_MARK) \
xx("\\{", TOK_CUBRACE_OPEN) \
xx("\\}", TOK_CUBRACE_CLOSE) \
xx("\\,", TOK_RESERVED_REGEX_CHAR) \
xx("\\+", TOK_RESERVED_REGEX_CHAR) \
xx("\\:", TOK_RESERVED_REGEX_CHAR) \
xx("\\;", TOK_SEMICOLON) \
xy("\\\\a", TOK_ESC_A, TOK_CHAR) \
xy("\\\\b", TOK_ESC_B, TOK_CHAR) \
xy("\\\\f", TOK_ESC_F, TOK_CHAR) \
xy("\\\\n", TOK_ESC_N, TOK_CHAR) \
xy("\\\\r", TOK_ESC_R, TOK_CHAR) \
xy("\\\\t", TOK_ESC_T, TOK_CHAR) \
xy("\\\\v", TOK_ESC_V, TOK_CHAR) \
xy("\\\\[\\\\\\{\\}\\[\\]\\(\\)\\,\\.\\^\\$\\*\\|\\?\\+\\:\\;\\-\\\'\\\"\\ ]", TOK_ESC_CHAR, TOK_CHAR) \
xy("\\\\x[0-9a-fA-F]", TOK_ESC_HEX1, TOK_CHAR) \
xy("\\\\x[0-9a-fA-F][0-9a-fA-F]", TOK_ESC_HEX2, TOK_CHAR) \
xy("\\\\[0-7]", TOK_ESC_OCT1, TOK_CHAR) \
xy("\\\\[0-7][0-7]", TOK_ESC_OCT2, TOK_CHAR) \
xy("\\\\[0-7][0-7][0-7]", TOK_ESC_OCT3, TOK_CHAR) \
xy("\\\\u\\{[0-9a-fA-F]+\\}", TOK_ESC_UNI1, TOK_CHAR) \
xy("\\\\u[0-9a-fA-F][0-9a-fA-F][0-9a-fA-F][0-9a-fA-F]", TOK_ESC_UNI2, TOK_CHAR) \
xy("\\\\p\\{[a-zA-Z_][0-9a-zA-Z_]*\\}", TOK_REGULAR_CATEGORY, TOK_CATEGORY) \
xy("\\\\P\\{[a-zA-Z_][0-9a-zA-Z_]*\\}", TOK_INVERTED_CATEGORY, TOK_CATEGORY) \
xx("\\\\A", TOK_ESC_START_OF_INPUT_ESC) \
xx("\\\\Z", TOK_ESC_END_OF_INPUT_ESC) \
xy("\\\\.", TOK_ESC_INVALID_ESCAPE, TOK_CHAR) \
xy("[ \\x9\\xb\\xc\\xd\\r\\n]+", TOK_WHITESPACE_CHAR, TOK_WHITESPACE) \
xx("[\\x20-\\x7e]", TOK_CHAR)


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

/* Switch to regular expressions (eg. limited tokens, escapes permitted) */
void tok_switch_to_regex(struct tkr_tokenizer *tkr);

const char *tok_token_type_to_str(token_type_t tkt);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TOKENS_H */
