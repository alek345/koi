#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Token* make_basic_token(Lexer *lexer, TokenType type) {
    Token *t = new Token();
    
    t->type = type;
    t->line_number = lexer->line_number;
    t->line_offset = lexer->line_offset;
    
    return t;
}

const char *token_type_to_string(TokenType type) {
    switch(type) {
#define TOKEN(tok) case tok: { return #tok ;}
        TOKEN(TOKEN_END);
        TOKEN(TOKEN_COMMA);
        TOKEN(TOKEN_COLON);
        TOKEN(TOKEN_SEMICOLON);
        TOKEN(TOKEN_LEFTPAR);
        TOKEN(TOKEN_RIGHTPAR);
        TOKEN(TOKEN_FUNCTION);
        TOKEN(TOKEN_IF);
        TOKEN(TOKEN_ELSE);
        TOKEN(TOKEN_INTEGER);
        TOKEN(TOKEN_FLOAT);
        TOKEN(TOKEN_STRING);
        TOKEN(TOKEN_IDENT);
        TOKEN(TOKEN_PLUS);
        TOKEN(TOKEN_MINUS);
        TOKEN(TOKEN_ASTRIX);
        TOKEN(TOKEN_SLASH);
        TOKEN(TOKEN_LESSTHAN);
        TOKEN(TOKEN_GREATERTHAN);
        TOKEN(TOKEN_EQUALS);
        TOKEN(TOKEN_VAR);
        TOKEN(TOKEN_RETURN);
		TOKEN(TOKEN_STRUCT);
		TOKEN(TOKEN_PIPE);
		TOKEN(TOKEN_LEFTBRACKET);
		TOKEN(TOKEN_RIGHTBRACKET);
		TOKEN(TOKEN_ENDIF);
		TOKEN(TOKEN_CFUNC);
		TOKEN(TOKEN_WHILE);
#undef TOKEN
    }
}

bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

bool is_num(char c) {
    return (is_digit(c) || c == '.');
}

bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') 
        || (c == '_');
}

bool is_alnum(char c) {
    return is_alpha(c) || is_num(c);
}

Lexer::Lexer(const char *path) {
    this->path = path;
    line_number = 1;
    line_offset = 0;
    num_tokens = 0;
    tokens = NULL;
}

void Lexer::ReadFile() {
    FILE *f = fopen(path, "rb");
    if(f == NULL) {
        assert(!"Failed to open file!");
    }
    
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    rewind(f);
    
    data = (char*) malloc(sizeof(char)*len+1);
    if(data == NULL) {
        assert(!"Failed to create text buffer");
    }
    
    fread(data, sizeof(char), len, f);
    data[len] = 0;
    
    ptr = data;
    
    fclose(f);
}

void Lexer::AddToken(Token *t) {
    num_tokens++;
    tokens = (Token**) realloc(tokens, sizeof(Token*)*num_tokens);
    tokens[num_tokens-1] = t;
}

void Lexer::Lex() {        
    ReadFile();
    
    while(*ptr != 0) {
        
        if(*ptr == '#') {
            ptr++;
            while(*ptr != '\n' || *ptr == '\r') {
                ptr++;
            }
            if(*ptr == '\n') {
                ptr++;
            }else {
                ptr += 2;
            }
            
            line_number++;
            line_offset = 0;
            continue;
        }
        
        if(*ptr == ' ') {
            ptr++;
            line_offset++;
            continue;
        }
        
        if(*ptr == '\t') {
            ptr++;
            line_offset += 4;
            continue;
        }
        
        if(*ptr == '\n') {
            ptr++;
            line_offset = 0;
            line_number++;
            continue;
        }
        
        if(*ptr == '\r') {
            ptr += 2;
            line_offset = 0;
            line_number++;
            continue;
        }
        
        if(*ptr == ',') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_COMMA));
            continue;
        }
        if(*ptr == ':') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_COLON));
            continue;
        }
		if (*ptr == '|') {
			ptr++;
			line_offset++;
			AddToken(make_basic_token(this, TOKEN_PIPE));
			continue;
		}
        if(*ptr == ';') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_SEMICOLON));
            continue;
        }
        if(*ptr == '(') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_LEFTPAR));
            continue;
        }
        if(*ptr == ')') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_RIGHTPAR));
            continue;
        }
		if (*ptr == '{') {
			ptr++;
			line_offset++;
			AddToken(make_basic_token(this, TOKEN_LEFTBRACKET));
			continue;
		}
		if (*ptr == '}') {
			ptr++;
			line_offset++;
			AddToken(make_basic_token(this, TOKEN_RIGHTBRACKET));
			continue;
		}
        if(*ptr == '+') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_PLUS));
            continue;
        }
        if(*ptr == '-') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_MINUS));
            continue;
        }
        if(*ptr == '*') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_ASTRIX));
            continue;
        }
        if(*ptr == '/') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_SLASH));
            continue;
        }
        if(*ptr == '<') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_LESSTHAN));
            continue;
        }
        if(*ptr == '>') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_GREATERTHAN));
            continue;
        }
        if(*ptr == '=') {
            ptr++;
            line_offset++;
            AddToken(make_basic_token(this, TOKEN_EQUALS));
            continue;
        }
        
        if(*ptr == '"') {
            ptr++;
            line_offset++;
            
            char *start = ptr;
            int len = 0;
            while(*ptr != '"') {
                len++;
                ptr++;
                line_offset++;
            }
            
            ptr++;
            
            char *str = (char*) malloc(sizeof(char)*len + 1);
            memcpy(str, start, len * sizeof(char));
            str[len] = 0;
            
            Token *t = new Token();
            t->type = TOKEN_STRING;
            t->line_number = line_number;
            t->line_offset = line_offset;
            t->strVal = str;
            
            AddToken(t);
            continue;
        }
            
        if(is_num(*ptr)) {
            bool found_dot = false;
            
            char *start = ptr;
            int len = 0;
            
            if(*ptr == '.') {
                found_dot = true;
                ptr++;
                line_offset++;
                len++;
            }
                
            while(is_num(*ptr)) {
                if(*ptr == '.') {
                    if(found_dot) {
                        printf("L%d:%d Found multiple '.' in number literal!\n", line_number, line_offset);
                        exit(-1);
                    } else {
                        found_dot = true;
                    }
                }
                len++;
                line_offset++;
                ptr++;
            }
            
            char *str = (char*) malloc(sizeof(char)*len + 1);
            memcpy(str, start, sizeof(char)*len);
            str[len] = 0;
            
            if(found_dot) {
                float num = strtof(str, NULL);
                // TODO: Check errno()
                Token *t = new Token();
                t->type = TOKEN_FLOAT;
                t->line_number = line_number;
                t->line_offset = line_offset;
                t->floatVal = num;
                AddToken(t);
            } else {
                int num = strtol(str, NULL, 10);
                // TODO: Checl errno()
                Token *t = new Token();
                t->type = TOKEN_INTEGER;
                t->line_number = line_number;
                t->line_offset = line_offset;
                t->intVal = num;
                AddToken(t);
            }
            
            free(str);
            continue;
        }
            
        if(is_alpha(*ptr)) {
            char *start = ptr;
            int len = 0;
            while(is_alnum(*ptr)) {
                len++;
                line_offset++;
                ptr++;
            }
            
            char *str = (char*) malloc(sizeof(char)*len + 1);
            memcpy(str, start, len * sizeof(char));
            str[len] = 0;
            
            Token *t = new Token();
            t->type = TOKEN_IDENT;
            t->line_number = line_number;
            t->line_offset = line_offset;
            t->strVal = str;
            
            AddToken(t);
            continue;
        }
            
        printf("L%d:%d Unknown token '%c'(0x%02X)\n", line_number, line_offset, *ptr, *ptr);
        exit(-1);
    }

    // Convert all idents that are keywords to specific keyword
    // tokens

    for(int i = 0; i < num_tokens; i++) {
        Token *t = tokens[i];
        
        if(t->type == TOKEN_IDENT) {
#define KEYWORD(s, toktype) if(strcmp(s, t->strVal) == 0) { t->type = toktype; free(t->strVal); continue; }
            KEYWORD("func", TOKEN_FUNCTION);
            KEYWORD("if", TOKEN_IF);
            KEYWORD("else", TOKEN_ELSE);
			KEYWORD("endif", TOKEN_ENDIF);
            KEYWORD("var", TOKEN_VAR);
            KEYWORD("return", TOKEN_RETURN);
			KEYWORD("struct", TOKEN_STRUCT);
			KEYWORD("cfunc", TOKEN_CFUNC);
			KEYWORD("while", TOKEN_WHILE);
#undef KEYWORD
        }
    }
    
    AddToken(make_basic_token(this, TOKEN_END));
    
    current_token = 0;
}

Token* Lexer::Current() {
    return tokens[current_token];
}

Token* Lexer::Peek() {
    if(current_token + 1 >= num_tokens) {
        return tokens[current_token];
    }
    return tokens[current_token+1];
}

Token* Lexer::Next() {
    if(current_token + 1 >= num_tokens) {
        return tokens[current_token];
    }
    return tokens[++current_token];
}
