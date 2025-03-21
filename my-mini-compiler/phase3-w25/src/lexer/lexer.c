/* lexer.c */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "../../include/lexer.h"

static int current_line = 1;
static char last_token_type = 'x';

// Keywords table
static struct {
    const char *word;
    TokenType type;
} keywords[] = {
    {"if", TOKEN_IF},
    {"int", TOKEN_INT},
    {"char", TOKEN_CHAR},
    {"string", TOKEN_STRING},
    {"float", TOKEN_FLOAT},
    {"double", TOKEN_DOUBLE},
    {"while", TOKEN_WHILE},
    {"repeat", TOKEN_REPEAT},
    {"while", TOKEN_UNTIL},
    {"factorial", TOKEN_FACTORIAL},
    {"print", TOKEN_PRINT},
};

static int is_keyword(const char *word) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(word, keywords[i].word) == 0) {
            return keywords[i].type;
        }
    }
    return 0;
}

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
        case ERROR_INVALID_IDENTIFIER:
            printf("Invalid identifier\n");
            break;
        case ERROR_UNEXPECTED_TOKEN:
            printf("Unexpected token '%s'\n", lexeme);
            break;
        default:
            printf("Unknown error\n");
    }
}

void print_token(Token token) {
    if (token.error != ERROR_NONE) {
        print_error(token.error, token.line, token.lexeme);
        return;
    }

    printf("Token: ");
    switch (token.type) {
        case TOKEN_NUMBER: printf("NUMBER");
            break;
        case TOKEN_PLUS: printf("PLUS");
            break;
        case TOKEN_MINUS: printf("MINUS");
            break;
        case TOKEN_STAR: printf("STAR");
            break;
        case TOKEN_SLASH: printf("SLASH");
            break;
        case TOKEN_IDENTIFIER: printf("IDENTIFIER");
            break;
        case TOKEN_ASSIGN: printf("ASSIGN");
            break;
        case TOKEN_NEQ: printf("NOT EQUAL TO");
            break;
        case TOKEN_EQ: printf("EQUALS");
            break;
        case TOKEN_GT: printf("GREATER THAN");
            break;
        case TOKEN_LT: printf("LESS THAN");
            break;
        case TOKEN_OR: printf("OR");
            break;
        case TOKEN_AND: printf("AND");
            break;
        case TOKEN_ADDRESS: printf("ADDRESS");
            break;
        case TOKEN_FACTORIAL: printf("FACTORIAL");
            break;
        case TOKEN_SEMICOLON: printf("SEMICOLON");
            break;
        case TOKEN_LPAREN: printf("LPAREN");
            break;
        case TOKEN_RPAREN: printf("RPAREN");
            break;
        case TOKEN_LBRACE: printf("LBRACE");
            break;
        case TOKEN_RBRACE: printf("RBRACE");
            break;
        case TOKEN_IF: printf("IF");
            break;
        case TOKEN_INT: printf("INT");
            break;
        case TOKEN_PRINT: printf("PRINT");
            break;
        case TOKEN_EOF: printf("EOF");
            break;
        case TOKEN_COMMA: printf("COMMA");
            break;
        default: printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n", token.lexeme, token.line);
}

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
        int dot = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
            if (c == '.') {
                if (dot) {
                    token.lexeme[i] = c;
                    token.error = ERROR_INVALID_NUMBER;
                }
                dot = 1;
            }
        } while ((isdigit(c) || c == '.') && i < sizeof(token.lexeme) - 1);

        if (isalpha(c) || c == '_') {
            token.error = ERROR_INVALID_IDENTIFIER;
            while (isalnum(c) && i < sizeof(token.lexeme) - 1) {
                token.lexeme[i++] = c;
                (*pos)++;
                c = input[*pos];
            }
            token.lexeme[i] = '\0';
            return token;
        }

        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        return token;
    }

    // Handle identifiers and keywords
    if (isalpha(c) || c == '_') {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            c = input[*pos];
        } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';

        // Check for keyword first
        if (is_keyword(token.lexeme)) {
            token.type = is_keyword(token.lexeme);
        } else {
            char temp_lexeme[sizeof(token.lexeme)];
            strncpy(temp_lexeme, token.lexeme, sizeof(token.lexeme));
            temp_lexeme[strlen(token.lexeme)] = '\0';

            int is_partial_keyword = 0;
            for (int j = 1; j < strlen(temp_lexeme); j++) {
                char temp_substr[sizeof(token.lexeme)];
                strncpy(temp_substr, temp_lexeme, j);
                temp_substr[j] = '\0';

                if (is_keyword(temp_substr)) {
                    is_partial_keyword = 1;
                    break;
                }
            }

            if (is_partial_keyword) {
                token.type = TOKEN_ERROR;
                token.error = ERROR_INVALID_IDENTIFIER;
            } else {
                token.type = TOKEN_IDENTIFIER;
            }
        }

        return token;
    }

    // Handle string literals
    if (c == '"' || c == '\'') {
        int i = 0;
        char quote_type = c;

        (*pos)++;
        while (input[*pos] != quote_type && input[*pos] != '\0' && i < sizeof(token.lexeme) - 1) {
            token.lexeme[i++] = input[*pos];
            (*pos)++;
        }

        if (input[*pos] == '\0') {
            token.error = ERROR_UNTERMINATED_STRING;
            return token;
        }

        (*pos)++; // Consume closing quote

        if (quote_type == '\'' && i > 1) {
            // Checking if char length is greater than one
            token.error = ERROR_CHAR_TOO_LONG;
            return token;
        }

        token.lexeme[i] = '\0';
        token.type = TOKEN_STRING;
        return token;
    }

    if (strchr("!=&|", c)) {
        int len = 1;
        if (c == '=') {
            token.type = TOKEN_ASSIGN;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            if (input[*pos + 1] == '=') {
                token.type = TOKEN_EQ;
                token.lexeme[1] = input[*pos + 1];
                token.lexeme[2] = '\0';
                len = 2;
            }
        } else if (c == '!') {
            token.type = TOKEN_FACTORIAL;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            if (input[*pos + 1] == '=') {
                token.type = TOKEN_NEQ;
                token.lexeme[1] = input[*pos + 1];
                token.lexeme[2] = '\0';
                len = 2;
            }
        } else if (c == '&') {
            token.type = TOKEN_ADDRESS;
            token.lexeme[0] = c;
            token.lexeme[1] = '\0';
            if (input[*pos + 1] == '&') {
                token.type = TOKEN_AND;
                token.lexeme[1] = input[*pos + 1];
                token.lexeme[2] = '\0';
                len = 2;
            }
        } else if (c == '|' && input[*pos + 1] == '|') {
            token.type = TOKEN_OR;
            token.lexeme[0] = c;
            token.lexeme[1] = input[*pos + 1];
            token.lexeme[2] = '\0';
            len = 2;
        }
        (*pos) += len;
        return token;
    }

    // Handle operators and delimiters
    (*pos)++;
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';

    switch (c) {
        case '+':
            token.type = TOKEN_PLUS;
            if (last_token_type == 'o') {
                token.error = ERROR_CONSECUTIVE_OPERATORS;
                return token;
            }
            last_token_type = 'o';
            break;
        case '-':
            token.type = TOKEN_MINUS;
            if (last_token_type == 'o') {
                token.error = ERROR_CONSECUTIVE_OPERATORS;
                return token;
            }
            last_token_type = 'o';
            break;
        case '*':
            token.type = TOKEN_STAR;
            if (last_token_type == 'o') {
                token.error = ERROR_CONSECUTIVE_OPERATORS;
                return token;
            }
            last_token_type = 'o';
            break;
        case '/':
            token.type = TOKEN_SLASH;
            if (last_token_type == 'o') {
                token.error = ERROR_CONSECUTIVE_OPERATORS;
                return token;
            }
            last_token_type = 'o';
            break;
        case '!':
            token.type = TOKEN_FACTORIAL;
            last_token_type = '!';
            break;
        case '<':
            token.type = TOKEN_LT;
            break;
        case '>':
            token.type = TOKEN_GT;
            break;
        case ';':
            token.type = TOKEN_SEMICOLON;
            break;
        case '(':
            token.type = TOKEN_LPAREN;
            break;
        case ')':
            token.type = TOKEN_RPAREN;
            break;
        case '{':
            token.type = TOKEN_LBRACE;
            break;
        case '}':
            token.type = TOKEN_RBRACE;
            break;
        case ',':
            token.type = TOKEN_COMMA;
        default:
            token.error = ERROR_INVALID_CHAR;
            break;
    }

    return token;
}

// int main() {
//     const char *input = "int x = 123;\n"   // Basic declaration and number
//                        "test_var = 456;\n"  // Identifier and assignment
//                        "print x;\n"         // Keyword and identifier
//                        "if (y > 10) {\n"    // Keywords, identifiers, operators
//                        "    @#$ invalid\n"  // Error case
//                        "    x = ++2;\n"     // Consecutive operator error
//                        "}";
//
//     printf("Analyzing input:\n%s\n\n", input);
//     int position = 0;
//     Token token;
//
//     do {
//         token = get_next_token(input, &position);
//         print_token(token);
//     } while (token.type != TOKEN_EOF);
//
//     return 0;
// }
