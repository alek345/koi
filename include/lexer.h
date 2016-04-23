#ifndef LEXER_H
#define LEXER_H

enum TokenType {
    TOKEN_END = -1,
    TOKEN_COMMA = 0,
    TOKEN_COLON,
    TOKEN_SEMICOLON,
    TOKEN_LEFTPAR,
    TOKEN_RIGHTPAR,
    TOKEN_FUNCTION,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_IDENT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTRIX,
    TOKEN_SLASH,
    TOKEN_LESSTHAN,
    TOKEN_GREATERTHAN,
    TOKEN_EQUALS,
    TOKEN_VAR,
    TOKEN_RETURN,
	TOKEN_STRUCT,
	TOKEN_PIPE,
	TOKEN_LEFTBRACKET,
	TOKEN_RIGHTBRACKET,

	// Used by expression parser
	TOKEN_FUNCCALL,
};

const char *token_type_to_string(TokenType type);

struct TokenList;
struct Token {
    TokenType type;
    int line_number = 0;
    int line_offset = 0;
    
    union {
        int intVal;
        float floatVal;
        char *strVal;
    };

	// Used by expression parser
	struct {
		int num_arguments;
		TokenList** arguments;
	} funccall;
};



struct Lexer {
    const char *path = 0;
    
    char *data = 0;
    char *ptr = 0;
    
    Token **tokens;
    int num_tokens;
    
    int line_number = 1;
    int line_offset = 0;
    
    int current_token;
    
    Lexer(const char *path);
    
    void ReadFile();
    void AddToken(Token  *t);
    void Lex();
    
    Token* Current();
    Token* Peek();
    Token* Next();
    // Maybe a reverse functin?
};

#endif /* LEXER_H */
