#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "scanner.h"

void initScanner(Scanner *scnr, const char *source) {
    scnr->start = source;
    scnr->current = source;
    scnr->start_line = 1;
    scnr->start_column = 0;
    scnr->current_column = 0;
    scnr->level = 0;
    scnr->indents[0] = 0;
    scnr->indent = 0;
    scnr->pending_dedents = 0;
    scnr->is_line_start = true;
}

static Token makeToken(Scanner const *scnr, TokenType type) {
    return (Token) {
        .type = type,
        .start = scnr->start,
        .length = (int)(scnr->current - scnr->start),
        .line = scnr->start_line,
        .column = scnr->start_column
    };
}

static Token errorToken(Scanner const *scnr, const char *message) {
    return (Token) {
        .type = TOKEN_ERROR,
        .start = message,
        .length = strlen(message),
        .line = scnr->start_line,
        .column = scnr->start_column
    };
}

static void markTokenStart(Scanner *scnr) {
    scnr->start = scnr->current;
    scnr->start_column = scnr->current_column;
}

static char advance(Scanner *scnr) {
    if (*scnr->current == '\n') {
        ++scnr->start_line;
        scnr->current_column = 0;
        scnr->is_line_start = true;
    } else {
        ++scnr->current_column;
        scnr->is_line_start = false;
    }

    return *scnr->current++;
}

static bool isAtEnd(Scanner const *scnr) {
    return *scnr->current == '\0';
}

static bool isSignificantWhitespace(Scanner const *const scnr) {
    return scnr->is_line_start && scnr->level == 0;
}

static bool hasPendingDedents(Scanner const *const scnr) {
    return scnr->indent != 0 || scnr->pending_dedents != 0;
}

static char peek(Scanner const *scnr) {
    return *scnr->current;
}

static char peekNext(Scanner const *scnr) {
    if (isAtEnd(scnr))
        return '\0';
    return scnr->current[1];
}

static char match(Scanner *scnr, const char expected) {
    if (peek(scnr) == expected) {
        advance(scnr);
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

static void skipWhitespace(Scanner *scnr) {
    for (;;) {
        switch (peek(scnr)) {
            case ' ':
            case '\t':
            case '\r':
                advance(scnr);
                break;
            case '\n':
                return;
            case '#':
                while (!isAtEnd(scnr) && peek(scnr) != '\n')
                    advance(scnr);
                return;
            default:
                return;
        }
    }
}

static Token name(Scanner *scnr) {
    while (isAlphanum(peek(scnr)) || peek(scnr) == '_')
        advance(scnr);

    return makeToken(scnr, TOKEN_NAME);
}

static Token string(Scanner *scnr) {
    char quote_char = advance(scnr);
    while (!isAtEnd(scnr)
        && peek(scnr) != '\n'
        && peek(scnr) != quote_char) {
        advance(scnr);
    }

    if (isAtEnd(scnr) || peek(scnr) == '\n')
        return errorToken(scnr, "unterminated string literal");

    advance(scnr); // consume the closing quotation mark.
    return makeToken(scnr, TOKEN_STRING);
}

static Token number(Scanner *scnr) {
    while (isDigit(peek(scnr)))
        advance(scnr);

    if (peek(scnr) == '.' && isDigit(peekNext(scnr))) {
        advance(scnr);
        while (isDigit(peek(scnr)))
            advance(scnr);
    }

    return makeToken(scnr, TOKEN_NUMBER);
}

typedef enum {
    INDENT_INCREMENT,
    INDENT_DECREMENT,
    INDENT_EXCEED,
    INDENT_ERROR,
    INDENT_NONE,
    INDENT_EMPTY
} IndentState;

static IndentState checkIndent(Scanner *scnr) {
    if (scnr->pending_dedents > 0)
        return INDENT_DECREMENT;

    int spaces = 0;
    for (;;) {
        if (match(scnr, ' '))
            ++spaces;
        else if (match(scnr, '\t'))
            spaces += 4;
        else
            break;
    }

    // handle empty lines.
    if (isWhitespace(peek(scnr))) {
        skipWhitespace(scnr);
        return INDENT_EMPTY;
    } else if (peek(scnr) == '\n') {
        return INDENT_EMPTY;
    }
    
    if (scnr->indents[scnr->indent] == spaces) {
        return INDENT_NONE;
    } else if (scnr->indents[scnr->indent] < spaces) {
        if (scnr->indent + 1 == MAX_INDENT)
            return INDENT_EXCEED;

        scnr->indents[++scnr->indent] = spaces;
        return INDENT_INCREMENT;
    } else {
        while (scnr->indents[scnr->indent] > spaces) {
            --scnr->indent;
            ++scnr->pending_dedents;
        }

        if (scnr->indents[scnr->indent] < spaces)
            return INDENT_ERROR;

        return INDENT_DECREMENT;
    }
}

Token scanToken(Scanner *scnr) {
    if (!isSignificantWhitespace(scnr)) {
        skipWhitespace(scnr);
    }

    markTokenStart(scnr);

    if (isAtEnd(scnr) && scnr->level != 0) {
        // report the error only once.
        scnr->level = 0;
        return errorToken(scnr, "EOF in multi-line statement");
    }

    while (isSignificantWhitespace(scnr)
        || (isAtEnd(scnr) && hasPendingDedents(scnr))) {
        IndentState state = checkIndent(scnr);

        if (state == INDENT_INCREMENT) {
            return makeToken(scnr, TOKEN_INDENT);
        } else if (state == INDENT_DECREMENT) {
            --scnr->pending_dedents;
            return makeToken(scnr, TOKEN_DEDENT);
        } else if (state == INDENT_EXCEED) { 
            return errorToken(scnr,
                "indents exceeded the maximum indentation limit");
        } else if (state == INDENT_ERROR) {
            return errorToken(scnr, "unexpected indent");
        } else if (state == INDENT_NONE) {
            markTokenStart(scnr);
            break;
        } else if (state == INDENT_EMPTY) {
            // consume insignificant newline character if any.
            match(scnr, '\n');
            markTokenStart(scnr);
            continue;
        }
    }

    if (isAtEnd(scnr)) {
        return makeToken(scnr, TOKEN_ENDMARKER);
    }

    if (peek(scnr) == '\n') {
        if (!isSignificantWhitespace(scnr)) {
            advance(scnr);
            return scanToken(scnr);
        } else {
            // emit token at the correct line and column.
            ++scnr->current_column;
            Token tok = makeToken(scnr, TOKEN_NEWLINE);
            advance(scnr);
            return tok;
        }
    }

    if (isDigit(peek(scnr))) return number(scnr);
    else if (isAlpha(peek(scnr))) return name(scnr);
    else if (peek(scnr) == '"' || peek(scnr) == '\'') return string(scnr);

    char c = advance(scnr);

    switch (c) {
        case '(':
            ++scnr->level;
            return makeToken(scnr, TOKEN_LPAR);
        case ')':
            --scnr->level;
            return makeToken(scnr, TOKEN_RPAR);
        case '{':
            ++scnr->level;
            return makeToken(scnr, TOKEN_LBRACE);
        case '}':
            --scnr->level;
            return makeToken(scnr, TOKEN_RBRACE);
        case '[':
            ++scnr->level;
            return makeToken(scnr, TOKEN_LSQB);
        case ']':
            --scnr->level;
            return makeToken(scnr, TOKEN_RSQB);
        case ':':
            return makeToken(scnr, TOKEN_COLON);
        case ';':
            return makeToken(scnr, TOKEN_SEMI);
        case ',':
            return makeToken(scnr, TOKEN_COMMA);
        case '.':
            if (peek(scnr) == '.' && peekNext(scnr) == '.')
                return makeToken(scnr, TOKEN_ELLIPSIS);
            return makeToken(scnr, TOKEN_DOT);
        case '+':
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_PLUSEQUAL : TOKEN_PLUS);
        case '-':
            if (match(scnr, '>')) return makeToken(scnr, TOKEN_RARROW);
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_MINEQUAL : TOKEN_MINUS);
        case '*': 
            if (match(scnr, '*'))
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_DOUBLESTAREQUAL : TOKEN_DOUBLESTAR);
            else
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_STAREQUAL : TOKEN_STAR);
        case '/':
            if (match(scnr, '/'))
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_DOUBLESLASHEQUAL : TOKEN_DOUBLESLASH);
            else
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_SLASHEQUAL : TOKEN_SLASH);
        case '@':
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_ATEQUAL : TOKEN_AT);
        case '%':
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_PERCENTEQUAL : TOKEN_PERCENT);
        case '|':
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_VBAREQUAL : TOKEN_VBAR);
        case '&':
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_AMPER : TOKEN_AMPEREQUAL);
        case '^':
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_CIRCUMFLEXEQUAL : TOKEN_CIRCUMFLEX);
        case '=':
            return makeToken(scnr,
                match(scnr, '=') ? TOKEN_EQEQUAL : TOKEN_EQUAL);
        case '<':
            if (match(scnr, '<'))
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_LEFTSHIFTEQUAL : TOKEN_LEFTSHIFT);
            else
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_LESSEQUAL : TOKEN_LESS);
        case '>':
            if (match(scnr, '>'))
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_RIGHTSHIFTEQUAL : TOKEN_RIGHTSHIFT);
            else
                return makeToken(scnr,
                    match(scnr, '=') ? TOKEN_GREATEREQUAL : TOKEN_GREATER);
        case '!':
            if (match(scnr, '='))
                return makeToken(scnr, TOKEN_NOTEQUAL);
        case '\\':
            if (match(scnr, '\n')) {
                scnr->is_line_start = false;
                return scanToken(scnr);
            }
            return errorToken(scnr,
                "unexpected character after line continuation character");
    }

    return errorToken(scnr, "unexpected character");
}
