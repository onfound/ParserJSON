//
// Created by ilyad on 26.05.2018.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "Parser.h"

static Token *tokensJSON;
static char *document;

static void (*startDocumentCallback)();

static void (*endDocumentCallback)();

static void (*startElementCallback)(TokenType tokenType);

static void (*endElementCallback)(TokenType tokenType);

static void (*charactersCallback)(Token token);

static Token *alloc_token(Parser *parser,
                   Token *tokens, size_t num_tokens) {
    Token *tok;
    if (parser->toknext >= (signed) num_tokens) {
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
    charactersCallback(*token);
}

static void subString(const char *string, int offset, int length, char **dst) {
    *dst = calloc((size_t) (length + 1), sizeof(char));
    for (int i = 0; i < length; ++i) {
        (*dst)[i] = string[offset + i];
    }
}


static int parse_different(Parser *parser, const char *js,
                           size_t len, Token *tokens, size_t num_tokens) {
    Token *token;
    int start = parser->position;
    TokenType tokenType;
    for (; parser->position < (signed) len && js[parser->position] != '\0'; parser->position++) {
        switch (js[parser->position]) {
            // определяем что токен не являющийся массиовм или объектом или
            // строкой завершился с помощью одного из следующих символов
            case '\t' :
            case '\r' :
            case '\n' :
            case ' ' :
            case ',' :
            case ']' :
            case '}' :
                goto found;
            default:
                break;
        }

        // бросаем ошибку потому что нашли символ который не входит в таблицу ASCII
        if (js[parser->position] < 32 || js[parser->position] >= 127) {
            parser->position = start;
            return ERROR_INVAL;
        }
    }

    //сюда попадем если нашли токен, знаем его старт и конец и валидность символов внутри токена
    found:

    //если режим подсчета токенов то ничего не делаем откатываем позицию и прерываем обработку токена
    if (!tokens) {
        parser->position--;
        return (TokenError) 0;
    }

    //Находим тип
    char *tokenString;
    int length = parser->position - start;
    subString(js, start, length, &tokenString);

    if (strcmp(tokenString, "null") == 0) tokenType = NULL_VALUE;
    else if (strcmp(tokenString, "true") == 0) tokenType = TRUE_VALUE;
    else if (strcmp(tokenString, "false") == 0) tokenType = FALSE_VALUE;
    else tokenType = NUMBER;
    startElementCallback(tokenType);
    if (tokenType == NUMBER) {
        //проверяем валидность числа
        // 1) начинается с цифры или со знака минус
        if (!(js[start] < 0x30 || js[start] > 0x39) || js[start] == '-') {
            // 2) если начинается с цифры 0 и если все число не является 0, то обязательно должна быть плавающая точка
            if (js[start] == '0' && start + 1 < parser->position && js[start + 1] != '.') return ERROR_NUMBER;
            //объявляем переменыые для уникальности одной точки в числе и експоненциальной формы
            int i = 0;
            bool hasFloat = false;
            bool hasExp = false;
            // смещаемся если был знак '-'
            if (js[start] == '-') {
                i = start + 1;
            } else {
                i = start;
            }
            // проверяем остальные числа
            for (; i < parser->position; ++i) {
                // если это не последний символ и он равен точке - проверяем что точки еще не было и экспоненциалной формы,
                // так как точка не может стоять после экспоненциальной формы, смещаемся если все норм :)
                if (i < parser->position - 1 && js[i] == '.') {
                    if (!hasFloat && !hasExp) {
                        i++;
                        hasFloat = true;
                    } else return ERROR_NUMBER;
                }
                //проверка экспоненциальной формы
                if (i < parser->position - 1 && (js[i] == 'e' || js[i] == 'E')) {
                    int offset = 1;
                    if (js[i + 1] == '-' || js[i + 1] == '+') offset++;
                    if (!hasExp) {
                        i += offset;
                        hasExp = true;
                    } else return ERROR_NUMBER;
                }
                //не является цифрой
                if (js[i] < 0x30 || js[i] > 0x39) {
                    return ERROR_NUMBER;
                }
            }
        } else return ERROR_INVAL;

    }

    //выделяем память под токен
    token = alloc_token(parser, tokens, num_tokens);

    //если выделенная память на кол-во токенов заполнена (исчерпали все кол-во токенов в аргументах парсера)
    if (!token) {
        parser->position = start;
        return ERROR_NOMEM;
    }

    //если все норм, то заполняем токен
    fill_token(token, tokenType, start, parser->position);
    // откатываем позицию назад

    endElementCallback(tokenType);
    parser->position--;

    return 0;
}

static int parse_string(Parser *parser, const char *js,
                        size_t len, Token *tokens, size_t num_tokens) {
    //токен строки
    Token *token;
    int start = parser->position;
    parser->position++;
    if (tokens) {
        startElementCallback(STRING);
    }
    for (; parser->position < (signed) len && js[parser->position] != '\0'; parser->position++) {
        //символ строки
        char c = js[parser->position];
        if (c == '\t' || c == '\n') {
            return ERROR_STRING;
        }
        //нашли кавычку
        if (c == '\"') {
            //если режим подсчета токенов, ниче не делаем возвращаем нолик :)
            if (!tokens) {
                return 0;
            }
            // выделяем память под токен стринга
            token = alloc_token(parser, tokens, num_tokens);
            // если исчерпали ресурс токенов (заданное кол-во)
            if (!token) {
                parser->position = start;
                return ERROR_NOMEM;
            }
            //заполняем токен
            fill_token(token, STRING, start + 1, parser->position);
            return 0;
        }
        // если нашли обратный слэш и после него есть какой то символ
        if (c == '\\' && parser->position + 1 < (signed) len) {
            int i;
            parser->position++;
            switch (js[parser->position]) {
                case '\"':
                case '/' :
                case '\\' :
                case 'b' :
                case 'f' :
                case 'r' :
                case 'n' :
                case 't' :
                    break;
                case 'u':
                    parser->position++;
                    for (i = 0; i < 4 && parser->position < (signed) len && js[parser->position] != '\0'; i++) {
                        if (!((js[parser->position] >= 48 && js[parser->position] <= 57) || /* проверяем 0-9 */
                              (js[parser->position] >= 65 && js[parser->position] <= 70) || /* проверяем A-F */
                              (js[parser->position] >= 97 && js[parser->position] <= 102))) { /* проверяем a-f */
                            parser->position = (unsigned int) start;
                            return ERROR_INVAL;
                        }
                        parser->position++;
                    }
                    parser->position--;
                    break;
                default:
                    parser->position = start;
                    return ERROR_BACKSLASH;
            }
        }
    }
    parser->position = (unsigned int) start;
    return ERROR_PART;
}


static int parse(Parser *parser, const char *js, size_t len,
                 Token *tokens, unsigned int countTokens) {
    HelpState helpState = UNDEFINED;
    TokenError r;
    int i;
    Token *tokenTemp;
    // Подсчет кол-ва токенов
    int count = 0;
    if (tokens)
        startDocumentCallback();
    //пока не уткнулись в конец строки или не превысили указанный диапазон строки передвигаемся посимвольно
    for (; parser->position < (signed) len && js[parser->position] != '\0'; parser->position++) {
        //определяем тип токена
        TokenType type;
        //текущий символ
        char c = js[parser->position];

        switch (c) {
            // Если начало токена массива или объекта
            case '{':
            case '[':
                // Мы нашли новый токен
                count++;
                // Если режим подсчета токенов, то посчитали и хватит :)
                if (!tokens) {
                    break;
                }
                if (c == '{')startElementCallback(OBJECT);
                else startElementCallback(ARRAY);
                // выделяем токен
                tokenTemp = alloc_token(parser, tokens, countTokens);
                // если превысили допустимое кол-во токенов
                if (!tokenTemp)
                    return ERROR_NOMEM;
                if (c == '{' && helpState != UNDEFINED) helpState = UNDEFINED;
                // если обрабатываем какой-то токен и нашли токен внутри него, то увеличиваем его кол-во детей
                if (parser->toksuper != -1) {
                    tokens[parser->toksuper].size++;
                }
                // определяем что же ве таки объект или массив
                tokenTemp->type = (c == '{' ? OBJECT : ARRAY);
                // устанавливаем токену метку начала токена в строке
                tokenTemp->start = parser->position;
                // ставим родителя предпоследний токен
                parser->toksuper = parser->toknext - 1;
                break;
                // Если закончился токен (массив/объект)
            case '}':
            case ']':
                // Если был режим подсчета, то ниче не делаем
                if (!tokens)
                    break;
                // определяем что тип токена
                type = (c == '}' ? OBJECT : ARRAY);
                if (c == '}') {
                    if (helpState == COLUMN || helpState == UNDEFINED) {
                        helpState = COLUMN;
                    } else return ERROR_OBJECT_PART;
                }
                // пробегаемся по всем найденым токенам
                for (i = parser->toknext - 1; i >= 0; i--) {
                    tokenTemp = &tokens[i];
                    // если начало существует а конец еще не определен
                    if (tokenTemp->start != -1 && tokenTemp->end == -1) {
                        // если найденный тип не соответсвует Пример: {[ }] скобка закончилась раньше
                        if (tokenTemp->type != type) {
                            return ERROR_PART;
                        }
                        // сбрасываем текущий обрабатываемый токен
                        parser->toksuper = -1;
                        // устанавливаем конец токена в строке

                        tokenTemp->end = parser->position + 1;
                        charactersCallback(*tokenTemp);
                        break;
                    }
                }
                // если не нашли обрабатываемый токен (когда кол-во закрывающих скобок больше начинающих)
                if (i == -1) return ERROR_PART;
                // доходим до конца массива токенов для того чтобы узнать позицию следующего обрабатываемого токена
                for (; i >= 0; i--) {
                    tokenTemp = &tokens[i];
                    // если не обработаный в плане границ токен то устанавливаем индекс текущего обрабатываемого токена
                    if (tokenTemp->start != -1 && tokenTemp->end == -1) {
                        // текущий индекс токена в массиве токенов который мы обрабатываем
                        parser->toksuper = i;
                        break;
                    }
                }
                endElementCallback(ARRAY);
                break;
                // Если нашли кавычку
            case '\"':

                // проверяем строку
                r = (TokenError) parse_string(parser, js, len, tokens, countTokens);
                if (r < 0)return r;

                // если поймали ошибку в формате строки то возвращаем код ошибки
                // токен строки считаем
                count++;
                if (!tokens) break;
                if (parser->toksuper == -1)
                    return ERROR_ROOT_TOKEN;
                if (helpState == UNDEFINED && tokens[parser->toksuper].type == OBJECT) helpState = COMMA;
                if (tokens[parser->toksuper].type == OBJECT) {
                    if (helpState == COMMA) helpState = KEY;
                    else if (helpState != COLUMN)
                        return ERROR_OBJECT_PART;
                }
                // если режим не подсчета токенов и предок есть то увеличиваем кол-во чайлдов
                if (parser->toksuper != -1)
                    tokens[parser->toksuper].size++;
                endElementCallback(STRING);
                break;
                // если управляющие символы или пробел то ничего не делаем
            case '\t' :
            case '\r' :
            case '\n' :
            case ' ':
                break;
                // нашли разделитель между ключем и значением
            case ':':
                //ставим родителя как предпоследний токен
                // parser->toksuper = parser->toknext - 1;
                if (!tokens)break;
                if (tokens[parser->toksuper].type != OBJECT) return ERROR_CHARACTER;
                if (helpState == KEY) helpState = COLUMN;
                else return ERROR_OBJECT_PART;
                break;
                // если нашли разграничитель между токенами
            case ',':
                // переопределяем ToSuper так как мы перечисляем токены в Toksuper
                // если режим не подсчета токенов и мы находимся не в массиве и не в объекте
                if (tokens && tokens[parser->toksuper].type == OBJECT) {
                    if (helpState == COLUMN) helpState = COMMA;
                    else {
                        return ERROR_OBJECT_PART;
                    }
                }
                if (tokens && tokens[parser->toksuper].type != ARRAY && tokens[parser->toksuper].type != OBJECT) {
                    // идем по всем токенам и находим самый близжайший массив или объект
                    for (i = parser->toknext - 1; i >= 0; i--) {
                        // если токен равен объекту или массиву
                        if (tokens[i].type == ARRAY || tokens[i].type == OBJECT) {
                            // если найденный токен не закрыт, то указываем родителя на него
                            if (tokens[i].start != -1 && tokens[i].end == -1) {
                                parser->toksuper = i;
                                break;
                            }
                        }
                    }
                }
                break;
                //если нашли что то другое
            default:
                //пробуем определить что это число/true/false/null
                r = (TokenError) parse_different(parser, js, len, tokens, countTokens);
                if (r < 0) return r;
                //увеличиваем кол-во токенов если режим подсчета
                count++;
                //увеличиваем кол-во чайлдов если режим не подсчета и мы обрабатываем какой то токен
                if (parser->toksuper != -1 && tokens)
                    tokens[parser->toksuper].size++;
                break;
        }
    }
    // если режим не поиска кол-ва токенов то проверяем корневой токен на тип
    if (tokens && (tokens[0].type != OBJECT && tokens[0].type != ARRAY && tokens[0].type != TRUE_VALUE
                   && tokens[0].type != FALSE_VALUE && tokens[0].type != NULL_VALUE)) {
        return ERROR_ROOT_TOKEN;
    }
    // пробегаемся по всем токенам если они есть и проверяем на завершенность обработки
    // токенов
    for (i = parser->toknext - 1; i >= 0; i--) {
        if (tokens[i].start != -1 && tokens[i].end == -1) {
            return ERROR_PART;
        }
    }
    if (tokens)
        endDocumentCallback();
    return count;
}


static void trim(const char *line, char *y, int *startName) {
    int newSize = N;
    for (int i = 0; i < N; ++i) {
        if (line[i] != '\n' || *startName == 1) {
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
        fprintf(stderr, "calloc() failed: insufficient memory!\n");
        return NULL;
    }
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}

static char *parseFromFile(char name[]) {
    FILE *fp;
    char line[N];
    char refactorLine[N];
    char *first = "";
    memset(refactorLine, 0, sizeof refactorLine);
    int startName = 0;
    fp = fopen(name, "r");
    if (fp == NULL) {
        perror("File not found");
        exit(1);
    }
    while (fgets(line, N, fp) != NULL) {
        trim(line, refactorLine, &startName);
        first = concat(first, refactorLine);
        memset(refactorLine, 0, sizeof refactorLine);
    }
    fclose(fp);
    return first;
}

static char *getJsonInline(char *path) {
    return parseFromFile(path); //create String JSON without space and \n
}


Token *getNextToken(Token token) {
    for (int i = 0; i < (signed) (sizeof(tokensJSON) / sizeof(Token)); ++i) {
        if (tokensJSON[i].start == token.start && tokensJSON[i].end == token.end) {
            return &tokensJSON[i + 1];
        }
    }
    return NULL;
}

void printToken(Token token) {
    char *result;
    int size = token.end - token.start;
    subString(document, token.start, size, &result);
    printf("%s", result);
    free(result);
}

static void init(Parser *parser) {
    parser->position = 0;
    parser->toknext = 0;
    parser->toksuper = -1;
}

static void throwError(int error) {
    switch (error) {
        case ERROR_ROOT_TOKEN:
            perror("Undefined JSON ROOT. A JSON payload should be an object or array");
            exit(ERROR_ROOT_TOKEN);
            break;
        case ERROR_NUMBER:
            perror("The number must be in the correct form");
            exit(ERROR_NUMBER);
            break;
        case ERROR_BACKSLASH:
            perror("Undefined control character");
            exit(ERROR_BACKSLASH);
            break;
        case ERROR_NOMEM:
            perror("cant allocate memory for tokens");
            exit(ERROR_NOMEM);
            break;
        case ERROR_INVAL:
            perror("Illegal Arguments!");
            exit(ERROR_INVAL);
            break;
        case ERROR_PART:
            perror("Incorrect part");
            exit(ERROR_PART);
            break;
        case ERROR_CHARACTER:
            perror("Undefined character ':'");
            exit(ERROR_CHARACTER);
            break;
        case ERROR_OBJECT_PART:
            perror("Object in JSON should be {\"key\": value, \"key\":value}");
            exit(ERROR_OBJECT_PART);
            break;
        case ERROR_STRING:
            perror("String is broken");
            exit(ERROR_STRING);
            break;;
        default:
            perror("Unsupported Error");
            exit(ERROR_UNDEFINED);
            break;
    }
}

/**
 * Parse JSON String & convert to Tokens
 * @return root Token (JSON)
 */

static Token *getJsonTokens() {
    char *jsonLine = document;
    int count; //count of Token
    int err; //value Error
    Parser p; //parser
    init(&p);
    // за первый проход считаем кол-во токенов для того чтобы выделить память
    count = parse(&p, jsonLine, strlen(jsonLine), NULL, 10);
    if (count < 0) {
        throwError(count);
    }
    // выделяем память для массива токенов
    tokensJSON = calloc((size_t) count, sizeof(Token));
    if (!tokensJSON) {
        throwError(ERROR_NOMEM);
    }
    init(&p);
    err = parse(&p, jsonLine, strlen(jsonLine), tokensJSON, (unsigned int) count);
    if (err < 0) {
        throwError(err);
    }
    return tokensJSON;
}

/**
 * Convert JSON text in file to inline text
 * Start parse JSON String & convert to Tokens
 * @return root Token (JSON)
 */

Token getJSON(char *JSON, void (*startDocument)(), void (*endDocument)(), void (*startElement)(TokenType),
              void (*endElement)(TokenType), void (*characters)(Token)) {
    startDocumentCallback = startDocument;
    endDocumentCallback = endDocument;
    startElementCallback = startElement;
    endElementCallback = endElement;
    charactersCallback = characters;
    document = getJsonInline(JSON);
    return getJsonTokens()[0];
}

void freeParserJSON(){
    free(tokensJSON);
    free(document);
}
