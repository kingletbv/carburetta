#ifndef DIRECTIVE_INPUT_SPLITTER_DEFS_H
#define DIRECTIVE_INPUT_SPLITTER_DEFS_H

#ifndef SITUS_H_INCLUDED
#define SITUS_H_INCLUDED
#include "situs.h"
#endif

// returned by scanner if the current line is a directive line (starts with %)
#define DIS_DIRECTIVE_LINE_AVAILABLE -1

// returned by scanner if the current line is not a directive line (starts with something other than % or is empty.)
#define DIS_SECTION_LINE_AVAILABLE -2

#endif /* DIRECTIVE_INPUT_SPLITTER_DEFS_H */
