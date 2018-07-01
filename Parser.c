//
// Created by ilyad on 26.05.2018.
//
#include <stdlib.h>
#include <mem.h>
#include <stdio.h>
#include "Parser.h"

static Token *tokensJSON;
static Token *alloc_token(Parser *parser,
                          Token *tokens, size_t num_tokens) {
    Token *tok;
    if (parser->toknext >= num_tokens) {
        return NULL;
    }
    tok = &tokens[parser->toknext++];
    tok->start = tok->end = -1;
    tok->size = 0;
    return tok;
}

static void fill_token(Token *token, TokenType type,
                       int start, int end) {
    token->type = type;
    token->start = start;
    token->end = end;
    token->size = 0;
}

static TokenError parse_different(Parser *parser, const char *js,
                                  size_t len, Token *tokens, size_t num_tokens) {
    Token *token;
    int start;
    start = parser->position;
    for (; parser->position < len && js[parser->position] != '\0'; parser->position++) {
        switch (js[parser->position]) {
            case '\t' :
            case '\r' :
            case '\n' :
            case ' ' :
            case ',' :
            case ']' :
            case '}' :
                goto found;
        }
        if (js[parser->position] < 32 || js[parser->position] >= 127) {
            parser->position = start;
            return ERROR_INVAL;
        }
    }
    found:
    if (tokens == NULL) {
        parser->position--;
        return 0;
    }
    token = alloc_token(parser, tokens, num_tokens);
    if (token == NULL) {
        parser->position = start;
        return ERROR_NOMEM;
    }
    fill_token(token, DIFFERENT, start, parser->position);
    parser->position--;
    return 0;
}

static TokenError parse_string(Parser *parser, const char *js,
                               size_t len, Token *tokens, size_t num_tokens) {
    Token *token;
    int start = parser->position;
    parser->position++;
    for (; parser->position < len && js[parser->position] != '\0'; parser->position++) {
        char c = js[parser->position];
        if (c == '\"') {
            if (tokens == NULL) {
                return 0;
            }
            token = alloc_token(parser, tokens, num_tokens);
            if (token == NULL) {
                parser->position = start;
                return ERROR_NOMEM;
            }
            fill_token(token, STRING, start + 1, parser->position);
            return 0;
        }
        if (c == '\\' && parser->position + 1 < len) {
            int i;
            parser->position++;
            switch (js[parser->position]) {
                case '\"':
                case '/' :
                case '\\' :
                case 'b' :
                case 'f' :
                case 'r' :
                case 'n'  :
                case 't' :
                    break;
                case 'u':
                    parser->position++;
                    for (i = 0; i < 4 && parser->position < len && js[parser->position] != '\0'; i++) {
                        if (!((js[parser->position] >= 48 && js[parser->position] <= 57) || /* проверяем 0-9 */
                              (js[parser->position] >= 65 && js[parser->position] <= 70) || /* проверяем A-F */
                              (js[parser->position] >= 97 && js[parser->position] <= 102))) { /* проверяем a-f */
                            parser->position = start;
                            return ERROR_INVAL;
                        }
                        parser->position++;
                    }
                    parser->position--;
                    break;
                default:
                    parser->position = start;
                    return ERROR_INVAL;
            }
        }
    }
    parser->position = start;
    return ERROR_PART;
}

static TokenError parse(Parser *parser, const char *js, size_t len,
                 Token *tokens, unsigned int countTokens) {
    TokenError r;
    int i;
    Token *tokenTemp;
    int count = 0;
    for (; parser->position < len && js[parser->position] != '\0'; parser->position++) {
        char c;
        TokenType type;
        c = js[parser->position];
        switch (c) {
            case '{':
            case '[':
                count++;
                if (tokens == NULL) { // режим подсчета токенов
                    break;
                }
                tokenTemp = alloc_token(parser, tokens, countTokens);
                if (tokenTemp == NULL)
                    return ERROR_NOMEM;
                if (parser->toksuper != -1) {
                    tokens[parser->toksuper].size++;
                }
                tokenTemp->type = (c == '{' ? OBJECT : ARRAY);
                tokenTemp->start = parser->position;
                parser->toksuper = parser->toknext - 1;
                break;
            case '}':
            case ']':
                if (tokens == NULL)
                    break;
                type = (c == '}' ? OBJECT : ARRAY);
                for (i = parser->toknext - 1; i >= 0; i--) {
                    tokenTemp = &tokens[i];
                    if (tokenTemp->start != -1 && tokenTemp->end == -1) {
                        if (tokenTemp->type != type) {
                            return ERROR_INVAL;
                        }
                        parser->toksuper = -1;
                        tokenTemp->end = parser->position + 1;
                        break;
                    }
                }
                if (i == -1) return ERROR_INVAL;
                for (; i >= 0; i--) {
                    tokenTemp = &tokens[i];
                    if (tokenTemp->start != -1 && tokenTemp->end == -1) {
                        parser->toksuper = i;
                        break;
                    }
                }
                break;
            case '\"':
                r = parse_string(parser, js, len, tokens, countTokens);
                if (r < 0) return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;
            case '\t' :
            case '\r' :
            case '\n' :
            case ' ':
                break;
            case ':':
                parser->toksuper = parser->toknext - 1;
                break;
            case ',':
                if (tokens != NULL &&
                    tokens[parser->toksuper].type != ARRAY &&
                    tokens[parser->toksuper].type != OBJECT) {
                    for (i = parser->toknext - 1; i >= 0; i--) {
                        if (tokens[i].type == ARRAY || tokens[i].type == OBJECT) {
                            if (tokens[i].start != -1 && tokens[i].end == -1) {
                                parser->toksuper = i;
                                break;
                            }
                        }
                    }

                }
                break;
            default:
                r = parse_different(parser, js, len, tokens, countTokens);
                if (r < 0) return r;
                count++;
                if (parser->toksuper != -1 && tokens != NULL)
                    tokens[parser->toksuper].size++;
                break;
        }
    }
    for (i = parser->toknext - 1; i >= 0; i--) {
        if (tokens[i].start != -1 && tokens[i].end == -1) {
            return ERROR_PART;
        }
    }
    return count;
}

static char *parseFromFile(char name[]) {
    FILE *fp;
    char line[N];
    char refactorLine[N];
    char *first = "";
    memset(refactorLine, 0, sizeof refactorLine);
    int startName = 0;
    fp = fopen(name, "r");
    while (fgets(line, N, fp) != NULL) {
        trim(line, refactorLine, &startName);
        first = concat(first, refactorLine);
        memset(refactorLine, 0, sizeof refactorLine);
    }
    fclose(fp);
    return first;
}


static void trim(char *line, char *y, int *startName) {
    int newSize = N;
    for (int i = 0; i < N; ++i) {
        if (line[i] != '\n') {
            if (line[i] == ' ') {
                if (*startName == 1) {
                    y[i - (N - newSize)] = line[i];
                } else newSize--;
            } else {
                y[i - (N - newSize)] = line[i];
            }
            if (line[i] == '\"') {
                if (*startName == 0) *startName = 1;
                else *startName = 0;
            }
        } else {
            break;
        }
    }
}

static char *concat(char *s1, char *s2) {

    size_t len1 = strlen(s1);
    size_t len2 = strlen(s2);
    char *result = calloc((size_t) len1 + len2 + 1, sizeof(char));
    if (!result) {
        fprintf(stderr, "malloc() failed: insufficient memory!\n");
        return NULL;
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}

char *getJsonLine(char *path) {
    return parseFromFile(path); //create String JSON without space and \n
}
Token getJSON(char *document){
    return getJsonTokens(document)[0];
}
static Token *getJsonTokens(char *jsonLine) {
    unsigned count; //count of Token
    int err; //value Error
    Parser p; //
    init(&p);
    count = parse(&p, jsonLine, strlen(jsonLine), NULL, 10);
    tokensJSON = calloc(count, sizeof(Token));
    if (tokensJSON == NULL) {
        perror("cant allocate memory for tokens!");
        exit(1);
    }
    init(&p);
    err = parse(&p, jsonLine, strlen(jsonLine), tokensJSON, count);
    return tokensJSON;
}

Token getValue(Token token){
    for (int i = 0; i < _msize(tokensJSON) / sizeof(Token); ++i) {
        if (tokensJSON[i].start == token.start && tokensJSON[i].end == token.end){
            return tokensJSON[i+1];
        }
    }
}

void printToken(char *document, Token token) {
    char *result;
    int size = token.end - token.start;
    subString(document, token.start, size, &result);
    printf(result);
}

Token *getChilds(Token token) {
    if (token.size == 0 || token.size == 1)
        return NULL;
    Token * childs;
    if (token.type == ARRAY) childs = calloc((size_t) token.size, sizeof(Token));
    else childs = calloc((size_t) token.size * 2, sizeof(Token));
    int countChild = 0;
    int tempRightBorder = 0;
    Token temp;
    for (int i = 0; i < _msize(tokensJSON)/ sizeof(Token); ++i) {
        temp = tokensJSON[i];
        if (temp.start > token.start && temp.end < token.end){
            if (temp.end > tempRightBorder){
                tempRightBorder = temp.end;
                childs[countChild] = temp;
                countChild++;
            }
        }
    }
    return childs;
}

Token *getChildKeys(Token parent) {
    Token* childs = getChilds(parent);
    Token *childsKey;
    int count = 0;
    if (childs == NULL)return NULL;
    else{
        for (int j = 0; j <  _msize(childs) / sizeof(Token) ; ++j) {
            if (childs[j].type == STRING && childs[j].size ==1) count++;
        }
        childsKey = calloc((size_t) count, sizeof(Token));
        count = 0;
        for (int i = 0; i < _msize(childs) / sizeof(Token); ++i) {
            if (childs[i].type == STRING && childs[i].size ==1) {
                childsKey[count] = childs[i];
                count++;
            }
        }
        return childsKey;
    }
}

static char *getValueStr(char *key, char *str, Token *tokens, int count) {
    char *a;
    char *result = NULL;
    for (int i = 0; i < count; ++i) {
        int size = tokens[i].end - tokens[i].start;
        subString(str, tokens[i].start, size, &a);
        if (strcmp(key, a) == 0) {
            size = tokens[i + 1].end - tokens[i + 1].start;
            subString(str, tokens[i + 1].start, size, &result);
        }
    }
    return result;
}

static void subString(const char *string, int offset, int length, char **dst) {
    *dst = calloc((size_t) (length + 1), sizeof(char));
    for (int i = 0; i < length; ++i) {
        (*dst)[i] = string[offset + i];
    }
}

static void init(Parser *parser) {
    parser->position = 0;
    parser->toknext = 0;
    parser->toksuper = -1;
}
