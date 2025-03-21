/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER, // e.g., "123", "456"
    //    TOKEN_OPERATOR,    // +, -, *, /
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_LT,     // <
    TOKEN_GT,       // >
    TOKEN_EQ,      // ==
    TOKEN_NEQ,     // !=
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_IDENTIFIER, // Variable names
    TOKEN_ASSIGN, // =
    TOKEN_SEMICOLON, // ;
    TOKEN_LPAREN, // (
    TOKEN_RPAREN, // )
    TOKEN_LBRACE, // {
    TOKEN_RBRACE, // }
    TOKEN_ADDRESS, // &
    TOKEN_IF, // if keyword
    TOKEN_INT, // int keyword
    TOKEN_CHAR,
    TOKEN_FLOAT,
    TOKEN_DOUBLE,
    TOKEN_WHILE, // while keyword
    TOKEN_REPEAT, // repeat keyword
    TOKEN_UNTIL, // until keyword
    TOKEN_FACTORIAL, // factorial keyword
    TOKEN_PRINT, // print keyword
    TOKEN_ERROR,
    TOKEN_COMMA, // ,
    TOKEN_STRING,
    TOKEN_KEYWORD
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
    ERROR_INVALID_OPERATOR,
    ERROR_UNEXPECTED_TOKEN
} ErrorType;

typedef struct {
    TokenType type;
    char lexeme[100]; // Actual text of the token
    int line; // Line number in source file
    ErrorType error; // Error type if any
} Token;

#endif /* TOKENS_H */