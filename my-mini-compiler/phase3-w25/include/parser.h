/* parser.h */
#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"

// Basic node types for AST
typedef enum {
    AST_PROGRAM,        // Program node
    AST_VARDECL,        // Variable declaration (int x)
    AST_ASSIGN,         // Assignment (x = 5)
    AST_PRINT,          // Print statement
    AST_NUMBER,         // Number literal
    AST_IDENTIFIER,     // Variable name
    AST_IF,
    AST_WHILE,
    AST_REPEAT,
    AST_BLOCK,
    AST_FACTORIAL,
    AST_ADDRESS_OF,
    AST_BINOP,
    AST_COMPARISONOP,
    AST_BOOLOP,
    AST_FUNCDECL,
    AST_PARAM
} ASTNodeType;

typedef enum {
    PARSE_ERROR_NONE,
    PARSE_ERROR_UNEXPECTED_TOKEN,
    PARSE_ERROR_MISSING_SEMICOLON,
    PARSE_ERROR_MISSING_IDENTIFIER,
    PARSE_ERROR_MISSING_EQUALS,
    PARSE_ERROR_INVALID_EXPRESSION,
    PARSE_ERROR_MISSING_PARENTHESIS,
    PARSE_ERROR_MISSING_CONDITION,
    PARSE_ERROR_MISSING_BLOCK_BRACES,
    PARSE_ERROR_INVALID_OPERATOR,
    PARSE_ERROR_FUNCTION_CALL_ERROR,
    PARSE_ERROR_UNDECLARED_VARIABLE
} ParseError;


// AST Node structure
typedef struct ASTNode {
    ASTNodeType type;           // Type of node
    Token token;               // Token associated with this node
    struct ASTNode* left;      // Left child
    struct ASTNode* right;     // Right child
    // TODO: Add more fields if needed
} ASTNode;

// Parser functions
void parser_init(const char* input);
ASTNode* parse(void);
void print_ast(ASTNode* node, int level);
void free_ast(ASTNode* node);

static ASTNode *parse_statement(void);
static ASTNode *parse_bool(void);
static ASTNode *parse_join(void);
static ASTNode *parse_equality(void);
static ASTNode *parse_relational(void);
static ASTNode *parse_expression(void);
static ASTNode *parse_term(void);
static ASTNode* parse_unary(void);

#endif /* PARSER_H */
