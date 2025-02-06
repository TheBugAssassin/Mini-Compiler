//
// Created by Youssef
//

/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TOKEN_EOF,        // End of file
    TOKEN_NUMBER,     // e.g., "123", "456"
    TOKEN_IDENTIFIER, // variable names, function names: foo, bar
    TOKEN_KEYWORD,    // reserved words: if, else, while
    TOKEN_STRING,     // string literals: "hello"
    TOKEN_OPERATOR,   // e.g., "+", "-"
    TOKEN_DELIMITER,  // delimiters: ;, (), {}
    TOKEN_COMMENT,    // comments: //, /* */
    TOKEN_SPECIAL,
    TOKEN_ERROR       // Token with an error
} TokenType;

typedef enum {
    ERROR_NONE,                    // no error
    ERROR_INVALID_CHAR,            // unrecogonized character
    ERROR_INVALID_NUMBER,          // Malinformed numeric literal
    ERROR_CONSECUTIVE_OPERATORS,   // Multiple consecutive operators
    ERROR_CHAR_TOO_LONG,           // Char exceeds limit
    ERROR_UNTERMINATED_STRING,     // string not properly closed                                                unimplemented
    ERROR_UNEXPECTED_EOF,          // unexpected end of file                                                    unimplemented
    ERROR_IDENTIFIER_TOO_LONG,     // identifier length exceeds limit                                           unimplemented
    ERROR_INVALID_IDENTIFIER,      // Identifier starting with a number or containing invalid characters        unimplemented
    ERROR_INVALID_OPERATOR
} ErrorType;

/* Token structure to store token information
 * TODO: Add more fields if needed for your implementation
 * Hint: You might want to consider adding line and column tracking if you want to debug your lexer properly.
 * Don't forget to update the token fields in lexer.c as well
 */
typedef struct {
    TokenType type;     // type of token
    char lexeme[256];   // actual text of the token (increased size for strings)
    int line;           // line number in source file
    int column;         // column number where token starts
    ErrorType error;    // error type if applicable
} Token;

#endif /* TOKENS_H */
