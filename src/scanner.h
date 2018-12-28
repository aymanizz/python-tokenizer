#ifndef SCANNER_H
#define SCANNER_H

#define MAX_INDENT 125

#include "token.h"

/* Scanner: represents the scanner state.
 *
 * @start: pointer marks the start of the current lexeme.
 * @current: points to the current character being looked at. when a token is
 *      recognized this points one past the last character in the lexeme.
 * @level: tracks level of paranthesis.
 * @indents: stack for tracking the indens.
 * @indent: last pushed indent index.
 * @pending_dedents: number of dedents pending to be emitted.
 * @is_line_start: true if at the line start otherwise false.
 */
typedef struct {
    const char *start;
    const char *current;
    int start_line;
    int start_column;
    int current_column;
    int level;
    int indents[MAX_INDENT];
    int indent;
    int pending_dedents;
    bool is_line_start;
} Scanner;

/* initScanner: initialize the global scanner.
 * 
 * @source: the source string to tokenize.
 * 
 * initialize the scanner to put it in a valid state. this must be called
 * before scanning tokens.
 */
void initScanner(Scanner *scanner, const char *source);

/* scanToken: scan a token and return it token.
 *
 * scan a token and return it. this alter's the inner state of the scanner.
 */
Token scanToken(Scanner *scanner);

#endif