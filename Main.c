#include <mem.h>
#include <stdio.h>
#include <malloc.h>
#include "Parser.h"

#define N 80

int main(void) {
    char *inlineJSON;
    Token *jsonTokens;
    Token *childKeys;
    Token token;
    Token token1;
    Token JSON;
    inlineJSON = getJsonInline("C:\\Users\\ilyad\\CLionProjects\\ParserJSON\\testsJSON\\fail7.json");

    JSON = getJSON(inlineJSON);
//    token = getValue(JSON);
    printToken(inlineJSON, JSON);
//    childKeys = getChildKeys(JSON);
//    token = getValue(childKeys[3]);
//    printf("%d : %d", token.start, token.end);
//    printf(inlineJSON);
//    printToken(inlineJSON, token);
//    printf("\n");
//    childKeys = getChilds(token);
//
//    printf("%d", _msize(childKeys) / sizeof(Token));
//    for (int i = 0; i < _msize(childKeys) / sizeof(Token); ++i) {
//        token = getValue(childKeys[i]);
//        printToken(inlineJSON, childKeys[i]);
//        printf("\n");
//    }







//    jsonTokens = getJsonTokens(inlineJSON); //получаем пропарсенный json
//    childKeys = getChildKeys(jsonTokens[10], jsonTokens); //получаем ключи всех детей
//    if (childKeys != NULL) { // проверяем если они есть
//        for (int i = 0; i < _msize(childKeys) / sizeof(Token); ++i) {
//            token = getValue(childKeys[i], jsonTokens);
//
//            printToken(inlineJSON, token);
//            printf("\n");
//        }
//    }


//    for (int i = 0; i < 25; ++i) {
//        printf("%d : ", jsonTokens[i].type);
//        for (int j = jsonTokens[i].start; j < jsonTokens[i].end; ++j) {
//            printf("%c", inlineJSON[j]);
//        }
//        printf(" at [%d;%d]  size = %d\n", jsonTokens[i].start, jsonTokens[i].end, jsonTokens[i].size);
//    }

//    printf("---------------------------------------------------------");
//    printf("\n");
//    Token *childs = getChilds(jsonTokens[0], jsonTokens);
//    if (childs != NULL) {
//        for (int i = 0; i < _msize(childs) / sizeof(Token); ++i) {
//            printf("%d : ", childs[i].type);
//            for (int v = childs[i].start; v < childs[i].end; ++v) {
//                printf("%c", inlineJSON[v]);
//            }
//            printf(" at [%d;%d]  size = %d\n", childs[i].start, childs[i].end, childs[i].size);
//        }
//    }
//    printToken(inlineJSON, childs[0]);
//    getKeys(token);
//    printf(getValueToken("productName",result, jsonTokens, count));

    printf("\n");
    return 0;
}
