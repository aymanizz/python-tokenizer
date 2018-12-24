#ifndef SCANNER_H
#define SCANNER_H

#define MAX_INDENT 125

#define FOREACH_TOKEN(FUNC) \
    FUNC(LPAR) \
    FUNC(RPAR) \
    FUNC(LSQB) \
    FUNC(RSQB) \
    FUNC(COLON) \
    FUNC(COMMA) \
    FUNC(SEMI) \
    FUNC(PLUS) \
    FUNC(MINUS) \
    FUNC(STAR) \
    FUNC(SLASH) \
    FUNC(VBAR) \
    FUNC(AMPER) \
    FUNC(CIRCUMFLEX) \
    FUNC(LESS) \
    FUNC(GREATER) \
    FUNC(EQUAL) \
    FUNC(DOT) \
    FUNC(PERCENT) \
    FUNC(LBRACE) \
    FUNC(RBRACE) \
    FUNC(AT) \
    FUNC(EQEQUAL) \
    FUNC(NOTEQUAL) \
    FUNC(LESSEQUAL) \
    FUNC(GREATEREQUAL) \
    FUNC(LEFTSHIFT) \
    FUNC(RIGHTSHIFT) \
    FUNC(DOUBLESTAR) \
    FUNC(PLUSEQUAL) \
    FUNC(MINEQUAL) \
    FUNC(STAREQUAL) \
    FUNC(SLASHEQUAL) \
    FUNC(PERCENTEQUAL) \
    FUNC(AMPEREQUAL) \
    FUNC(VBAREQUAL) \
    FUNC(CIRCUMFLEXEQUAL) \
    FUNC(LEFTSHIFTEQUAL) \
    FUNC(RIGHTSHIFTEQUAL) \
    FUNC(DOUBLESTAREQUAL) \
    FUNC(DOUBLESLASH) \
    FUNC(DOUBLESLASHEQUAL) \
    FUNC(ATEQUAL) \
    FUNC(RARROW) \
    FUNC(ELLIPSIS) \
    FUNC(NAME) \
    FUNC(STRING) \
    FUNC(NUMBER) \
    FUNC(AWAIT) \
    FUNC(ASYNC) \
    FUNC(NL) \
    FUNC(TILDE) \
    FUNC(NEWLINE) \
    FUNC(INDENT) \
    FUNC(DEDENT) \
    FUNC(ENDMARKER) \
    FUNC(ERROR) \
    FUNC(NT_OFFSET) \
    FUNC(ENCODING) \

typedef enum {
    #define EXPAND(NAME) TOKEN_ ## NAME,
    FOREACH_TOKEN(EXPAND)
    #undef EXPAND
} TokenType;

/* Token: represents a token.
 *
 * @type: token type.
 * @start: a pointer to the start of the token lexeme in the source string.
 * @length: length of the token lexeme.
 * @line: line at which the token lexeme starts.
 * @column: column at which the token lexeme starts.
 */
typedef struct {
    TokenType type;
    const char *start;
    int length;
    int line, column;
} Token;

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

/* printToken: helper function for printing a token representation.
 *
 * prints the token in the format: <line>, <column> <type name> <lexeme>
 */
void printToken(Token token);

#endif