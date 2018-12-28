#ifndef TOKEN_H
#define TOKEN_H

/* AUTO-GENERATED DO NOT MODIFY. */
typedef enum {
    TOKEN_LPAR,
    TOKEN_RPAR,
    TOKEN_LSQB,
    TOKEN_RSQB,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_SEMI,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_VBAR,
    TOKEN_AMPER,
    TOKEN_CIRCUMFLEX,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_EQUAL,
    TOKEN_DOT,
    TOKEN_PERCENT,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_AT,
    TOKEN_EQEQUAL,
    TOKEN_NOTEQUAL,
    TOKEN_LESSEQUAL,
    TOKEN_GREATEREQUAL,
    TOKEN_LEFTSHIFT,
    TOKEN_RIGHTSHIFT,
    TOKEN_DOUBLESTAR,
    TOKEN_PLUSEQUAL,
    TOKEN_MINEQUAL,
    TOKEN_STAREQUAL,
    TOKEN_SLASHEQUAL,
    TOKEN_PERCENTEQUAL,
    TOKEN_AMPEREQUAL,
    TOKEN_VBAREQUAL,
    TOKEN_CIRCUMFLEXEQUAL,
    TOKEN_LEFTSHIFTEQUAL,
    TOKEN_RIGHTSHIFTEQUAL,
    TOKEN_DOUBLESTAREQUAL,
    TOKEN_DOUBLESLASH,
    TOKEN_DOUBLESLASHEQUAL,
    TOKEN_ATEQUAL,
    TOKEN_RARROW,
    TOKEN_ELLIPSIS,
    TOKEN_NAME,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_AWAIT,
    TOKEN_ASYNC,
    TOKEN_NL,
    TOKEN_TILDE,
    TOKEN_NEWLINE,
    TOKEN_INDENT,
    TOKEN_DEDENT,
    TOKEN_ENDMARKER,
    TOKEN_ERROR,
    TOKEN_NT_OFFSET,
    TOKEN_ENCODING,
    TOKEN_AND,
    TOKEN_AS,
    TOKEN_ASSERT,
    TOKEN_BREAK,
    TOKEN_CLASS,
    TOKEN_CONTINUE,
    TOKEN_DEF,
    TOKEN_DEL,
    TOKEN_ELIF,
    TOKEN_ELSE,
    TOKEN_EXCEPT,
    TOKEN_FALSE,
    TOKEN_FINALLY,
    TOKEN_FOR,
    TOKEN_FROM,
    TOKEN_GLOBAL,
    TOKEN_IF,
    TOKEN_IMPORT,
    TOKEN_IN,
    TOKEN_IS,
    TOKEN_LAMBDA,
    TOKEN_NONE,
    TOKEN_NONLOCAL,
    TOKEN_NOT,
    TOKEN_OR,
    TOKEN_PASS,
    TOKEN_RAISE,
    TOKEN_RETURN,
    TOKEN_TRUE,
    TOKEN_TRY,
    TOKEN_WHILE,
    TOKEN_WITH,
    TOKEN_YIELD,
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

/* table of token names */
extern const char * const Token_Names[];

#endif