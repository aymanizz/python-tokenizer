#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "scanner.h"

Scanner scanner;

void initScanner(const char *source) {
    scanner.start = source;
    scanner.current = source;
    scanner.start_line = 1;
    scanner.start_column = 0;
    scanner.current_column = 0;
    scanner.level = 0;
    scanner.indents[0] = 0;
    scanner.indent = 0;
    scanner.pending_dedents = 0;
    scanner.is_line_start = true;
}

static Token makeToken(TokenType type) {
    return (Token) {
        .type = type,
        .start = scanner.start,
        .length = (int)(scanner.current - scanner.start),
        .line = scanner.start_line,
        .column = scanner.start_column
    };
}

static Token errorToken(const char *message) {
    return (Token) {
        .type = TOKEN_ERROR,
        .start = message,
        .length = strlen(message),
        .line = scanner.start_line,
        .column = scanner.start_column
    };
}

static void markTokenStart() {
    scanner.start = scanner.current;
    scanner.start_column = scanner.current_column;
}

static char advance() {
    if (*scanner.current == '\n') {
        ++scanner.start_line;
        scanner.current_column = 0;
        scanner.is_line_start = true;
    } else {
        ++scanner.current_column;
        scanner.is_line_start = false;
    }

    return *scanner.current++;
}

static bool isAtEnd() {
    return *scanner.current == '\0';
}

static char peek() {
    return *scanner.current;
}

static char peekNext() {
    if (isAtEnd())
        return '\0';
    return scanner.current[1];
}

static char match(const char expected) {
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

static bool isDigit(const char c) {
    return c >= '0' && c <= '9';
}

static bool isAlpha(const char c) {
    return ((c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z'));
}

static bool isAlphanum(const char c) {
    return isAlpha(c) || isDigit(c);
}

static bool isWhitespace(const char c) {
    return c == '\t' || c == '\r'
        || c == ' ' || c == '#';
}

static void skipWhitespace() {
    for (;;) {
        switch (peek()) {
            case ' ':
            case '\t':
            case '\r':
                advance();
                break;
            case '\n':
                return;
            case '#':
                while (!isAtEnd() && peek() != '\n')
                    advance();
                if (!isAtEnd())
                    advance(); // consume newline.
                return;
            default:
                return;
        }
    }
}

static Token name() {
    while (isAlphanum(peek()))
        advance();

    return makeToken(TOKEN_NAME);
}

static Token string() {
    while (!isAtEnd() && peek() != '\n' && peek() != '"') {
        advance();
    }

    if (isAtEnd() || peek() == '\n')
        return errorToken("unterminated string literal");

    advance(); // consume the closing quotation mark.
    return makeToken(TOKEN_STRING);
}

static Token number() {
    while (isDigit(peek())) advance();

    if (peek() == '.' && isDigit(peekNext())) {
        advance();
        while (isDigit(peek())) advance();
    }

    return makeToken(TOKEN_NUMBER);
}

typedef enum {
    INDENT_INCREMENT,
    INDENT_DECREMENT,
    INDENT_EXCEED,
    INDENT_ERROR,
    INDENT_NONE
} IndentState;

static IndentState checkIndent() {
    if (scanner.pending_dedents < 0)
        return INDENT_DECREMENT;

    int spaces;
    while (true) {
        spaces = 0;
        while (peek() == ' ' || peek() == '\t') {
            if (advance() == ' ')
                ++spaces;
            else
                spaces += 4;
        }

        if (isWhitespace(peek())) {
            skipWhitespace();
            markTokenStart();
        } else if (peek() == '\n') {
            advance();
            markTokenStart();
        } else if (isAtEnd()) {
            return INDENT_NONE;
        } else {
            break;
        }
    }
    
    if (scanner.indents[scanner.indent] == spaces) {
        return INDENT_NONE;
    } else if (scanner.indents[scanner.indent] < spaces) {
        if (scanner.indent + 1 == MAX_INDENT)
            return INDENT_EXCEED;

        scanner.indents[++scanner.indent] = spaces;
        return INDENT_INCREMENT;
    } else {
        while (scanner.indents[scanner.indent] > spaces) {
            --scanner.indent;
            --scanner.pending_dedents;
        }

        if (scanner.indents[scanner.indent] < spaces)
            return INDENT_ERROR;

        return INDENT_DECREMENT;
    }
}

Token scanToken() {
next_token:
    if (!scanner.is_line_start || scanner.level != 0) {
        skipWhitespace();
    }

    markTokenStart();

    // scanner.is_line_start can be set to true by the previous
    // call to skipWhiteSpace.
    if (scanner.is_line_start && scanner.level == 0)
    indent_check_start_no_condition: {
        IndentState state = checkIndent();

        switch (state) {
            case INDENT_INCREMENT:
                return makeToken(TOKEN_INDENT);
            case INDENT_DECREMENT:
                ++scanner.pending_dedents;
                return makeToken(TOKEN_DEDENT);
            case INDENT_EXCEED:
                return errorToken(
                    "indents exceeded the maximum indentation limit");
            case INDENT_ERROR:
                return errorToken("unexpected indent");
            case INDENT_NONE:
                break;
        }
    }

    markTokenStart();

    if (isAtEnd()) {
        if (scanner.indent != 0 || scanner.pending_dedents != 0) {
            goto indent_check_start_no_condition;
        }
        return makeToken(TOKEN_ENDMARKER);
    }

    char c = advance();

    if (isDigit(c)) return number();
    else if (isAlpha(c)) return name();
    else if (c == '"') return string();
    
    switch (c) {
        case '(':
            ++scanner.level;
            return makeToken(TOKEN_LPAR);
        case ')':
            --scanner.level;
            return makeToken(TOKEN_RPAR);
        case '{':
            ++scanner.level;
            return makeToken(TOKEN_LBRACE);
        case '}':
            --scanner.level;
            return makeToken(TOKEN_RBRACE);
        case '[':
            ++scanner.level;
            return makeToken(TOKEN_LSQB);
        case ']':
            --scanner.level;
            return makeToken(TOKEN_RSQB);
        case ':':
            return makeToken(TOKEN_COLON);
        case ';':
            return makeToken(TOKEN_SEMI);
        case ',':
            return makeToken(TOKEN_COMMA);
        case '.':
            if (peek() == '.' && peekNext() == '.')
                return makeToken(TOKEN_ELLIPSIS);
            return makeToken(TOKEN_DOT);
        case '+':
            return makeToken(match('=') ? TOKEN_PLUSEQUAL : TOKEN_PLUS);
        case '-':
            if (match('>')) return makeToken(TOKEN_RARROW);
            return makeToken(match('=') ? TOKEN_MINEQUAL : TOKEN_MINUS);
        case '*': 
            if (match('*'))
                return makeToken(
                    match('=') ? TOKEN_DOUBLESTAREQUAL : TOKEN_DOUBLESTAR);
            else
                return makeToken(match('=') ? TOKEN_STAREQUAL : TOKEN_STAR);
        case '/':
            if (match('/'))
                return makeToken(
                    match('=') ? TOKEN_DOUBLESLASHEQUAL : TOKEN_DOUBLESLASH);
            else
                return makeToken(match('=') ? TOKEN_SLASHEQUAL : TOKEN_SLASH);
        case '@':
            return makeToken(match('=') ? TOKEN_ATEQUAL : TOKEN_AT);
        case '%':
            return makeToken(match('=') ? TOKEN_PERCENTEQUAL : TOKEN_PERCENT);
        case '|':
            return makeToken(match('=') ? TOKEN_VBAREQUAL : TOKEN_VBAR);
        case '&':
            return makeToken(match('=') ? TOKEN_AMPER : TOKEN_AMPEREQUAL);
        case '^':
            return makeToken(
                match('=') ? TOKEN_CIRCUMFLEXEQUAL : TOKEN_CIRCUMFLEX);
        case '=':
            return makeToken(match('=') ? TOKEN_EQEQUAL : TOKEN_EQUAL);
        case '<':
            if (match('<'))
                return makeToken(
                    match('=') ? TOKEN_LEFTSHIFTEQUAL : TOKEN_LEFTSHIFT);
            else
                return makeToken(
                    match('=') ? TOKEN_LESSEQUAL : TOKEN_LESS);
        case '>':
            if (match('>'))
                return makeToken(
                    match('=') ? TOKEN_RIGHTSHIFTEQUAL : TOKEN_RIGHTSHIFT);
            else
                return makeToken(
                    match('=') ? TOKEN_GREATEREQUAL : TOKEN_GREATER);
        case '!':
            if (match('='))
                return makeToken(TOKEN_NOTEQUAL);
        case '\\':
            if (match('\n')) {
                scanner.is_line_start = false;
                goto next_token;
            }
            return errorToken(
                "unexpected character after line continuation character");
        case '\n':
            if (scanner.level > 0) {
                goto next_token;
            } else {
                return makeToken(TOKEN_NEWLINE);
            }
    }

    return errorToken("unexpected character");
}

void printToken(Token token) {
    #define HANDLE_CASE(TOKEN) \
        case TOKEN_ ## TOKEN: \
            printf("%02i, %02i: \t %-16s \'%.*s\'\n", \
                token.line, token.column, #TOKEN, token.length, token.start); \
            break;

    if (token.type == TOKEN_NEWLINE) {
        printf("%02i, %02i: \t %-16s\n", token.line, token.column, "NEWLINE");
        return;
    }

    switch (token.type) {
        FOREACH_TOKEN(HANDLE_CASE)
    }

    #undef HANDLE_CASE
}
