//
// Created by ilyad on 26.05.2018.
//

#ifndef __PARSER_H_
#define __PARSER_H_

#include <stddef.h>

#define N 80

typedef enum {
    NUMBER = 0,
    OBJECT = 1,
    ARRAY = 2,
    STRING = 3,
    TRUE_VALUE = 4,
    FALSE_VALUE = 5,
    NULL_VALUE = 6
} TokenType;

typedef enum {
    ERROR_NOMEM = -1,
    ERROR_INVAL = -2,
    ERROR_PART = -3
} TokenError;

typedef struct {
    TokenType type;
    int start;
    int end;
    int size;
} Token;

typedef struct {
    unsigned int position;
    unsigned int toknext;
    int toksuper;
} Parser;

static void trim(char *line, char *y, int *startName);

static char *concat(char *s1, char *s2);

static char *parseFromFile(char name[]);

static void init(Parser *parser);

char *getJsonInline(char *path);

static Token *getJsonTokens(char *jsonLine);

Token *getChildKeys(Token parent);

void printToken(char *document, Token token);

Token *getChilds(Token token);

Token getValue(Token token);

static char *getValueStr(char *key, char *str, Token *tokens, int count);
Token getJSON(char *inlineJSON);

static void subString(const char *string, int offset, int length, char **dst);

static TokenError parse(Parser *parser, const char *js, size_t len,
                 Token *tokens, unsigned int countTokens);

#endif

