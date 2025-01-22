/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../../include/tokens.h"

// Line tracking
static int current_line = 1;
static char last_token_type = 'x'; // For checking consecutive operators

const char *keywords[] = {"if", "repeat", "until", "else", "while", NULL};

/* Print error messages for lexical errors */
void print_error(ErrorType error, int line, const char *lexeme) {
    printf("Lexical Error at line %d: ", line);
    switch (error) {
        case ERROR_INVALID_CHAR:
            printf("Invalid character '%s'\n", lexeme);
            break;
        case ERROR_INVALID_NUMBER:
            printf("Invalid number format\n");
            break;
        case ERROR_CONSECUTIVE_OPERATORS:
            printf("Consecutive operators not allowed\n");
            break;
        case ERROR_UNTERMINATED_STRING:
            printf("Unterminated string literal\n");
            break;
        default:
            printf("Unknown error\n");
    }
}

/* Print token information
 *
 *  TODO Update your printing function accordingly
 */

/* Print token information */
void print_token(Token token) {
    if (token.error != ERROR_NONE) {
        print_error(token.error, token.line, token.lexeme);
        return;
    }
    printf("Token: ");
    switch (token.type) {
        case TOKEN_NUMBER:
            printf("NUMBER");
            break;
        case TOKEN_IDENTIFIER:
            printf("IDENTIFIER");
            break;
        case TOKEN_KEYWORD:
            printf("KEYWORD");
            break;
        case TOKEN_STRING:
            printf("STRING");
            break;
        case TOKEN_OPERATOR:
            printf("OPERATOR");
            break;
        case TOKEN_DELIMITER:
            printf("DELIMITER");
            break;
        case TOKEN_EOF:
            printf("EOF");
            break;
        default:
            printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n", token.lexeme, token.line);
}

/* Check if string is a keyword */
int is_keyword(const char *str) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}


/* Get next token from input */
Token get_next_token(const char *input, int *pos) {
    Token token = {TOKEN_ERROR, "", current_line, ERROR_NONE};
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
        if (c == '\n') {
            current_line++;
        }
        (*pos)++;
    }

    if (input[*pos] == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    c = input[*pos];

    // Handle comments
    if (c == '/' && input[*pos + 1] == '/') {
        while (input[*pos] != '\n' && input[*pos] != '\0') {
            (*pos)++;
        }
        return get_next_token(input, pos);
    }
    if (c == '/' && input[*pos + 1] == '*') {
        (*pos) += 2;
        while (!(input[*pos] == '*' && input[*pos + 1] == '/') && input[*pos] != '\0') {
            if (input[*pos] == '\n') current_line++;
            (*pos)++;
        }
        if (input[*pos] != '\0') (*pos) += 2;
        return get_next_token(input, pos);
    }

    // Handle numbers
    if (isdigit(c)) {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // Handle identifiers and keywords
    if (isalpha(c)) {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while (isalnum(c) && i < sizeof(token.lexeme) - 1);
        token.lexeme[i] = '\0';
        token.type = is_keyword(token.lexeme) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
        return token;
    }

    // Handle string literals
    if (c == '"') {
        int i = 0;
        (*pos)++;
        while ((c = input[*pos]) != '"' && c != '\0' && i < sizeof(token.lexeme) - 1) {
            token.lexeme[i++] = c;
            (*pos)++;
        }
        if (c == '\0') {
            token.error = ERROR_UNTERMINATED_STRING;
        } else {
            (*pos)++; // Consume closing quote
            token.lexeme[i] = '\0';
            token.type = TOKEN_STRING;
        }
        return token;
    }

    // Handle operators
    if (c == '+' || c == '-') {
        if (last_token_type == 'o') {
            // Check for consecutive operators
            token.error = ERROR_CONSECUTIVE_OPERATORS;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            (*pos)++;
            return token;
        }
        token.type = TOKEN_OPERATOR;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        last_token_type = 'o';
        (*pos)++;
        return token;
    }

    // Handle delimiters
    if (strchr(";(){}", c)) {
        token.type = TOKEN_DELIMITER;
        token.lexeme[0] = c;
        token.lexeme[1] = '\0';
        (*pos)++;
        return token;
    }

    // Handle invalid characters
    token.error = ERROR_INVALID_CHAR;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    (*pos)++;
    return token;
}

// This is a basic lexer that handles numbers (e.g., "123", "456"), basic operators (+ and -), consecutive operator errors, whitespace and newlines, with simple line tracking for error reporting.

int main() {
    const char *input = "123 + 456 - 789\n1 ++ 2"; // Test with multi-line input
    int position = 0;
    Token token;

    printf("Analyzing input:\n%s\n\n", input);

    do {
        token = get_next_token(input, &position);
        print_token(token);
    } while (token.type != TOKEN_EOF);

    return 0;
}

//gcc ./phase1-w25/src/lexer/lexer.c -o lexer.exe
//.\lexer.exe