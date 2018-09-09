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
    ERROR_ROOT_TOKEN = -1,
    ERROR_NUMBER = -2,
    ERROR_BACKSLASH = -3,
    ERROR_OBJECT_PART = -4,
    ERROR_CHARACTER = -5,
    ERROR_STRING = -6,
    ERROR_NOMEM = -7,
    ERROR_INVAL = -8,
    ERROR_PART = -9,
    ERROR_ALLOCATE = -10
} TokenError;

typedef struct {
    TokenType type;
    int start;
    int end;
    int size;
} Token;

typedef enum {
    UNDEFINED = 0,
    KEY = 1,
    COLUMN = 2,
    COMMA = 3
} HelpState;

typedef struct {
    int position;
    int toknext;
    int toksuper;
} Parser;

void trim(char *line, char *y, int *startName);

char *concat(char *s1, char *s2);

char *parseFromFile(char name[]);

void init(Parser *parser);

char *getJsonInline(char *path);

Token *getJsonTokens();

//Token *getChildKeys(Token parent);

void printToken(Token token);

//Token *getChilds(Token token);

//Token getValue(Token token);

void throwError(int error);

//static char *getValueStr(char *key, char *str, Token *tokens, int count);

Token getJSON(char *JSON, void (*startDocument)(), void (*endDocument)()
        , void (*startElement)(TokenType), void (*endElement)(TokenType), void (*characters)(Token));

void subString(const char *string, int offset, int length, char **dst);

TokenError parse(Parser *parser, const char *js, size_t len,
                 Token *tokens, unsigned int countTokens);

#endif

