#include <mem.h>
#include <stdio.h>
#include <malloc.h>
#include "Parser.h"

#define N 80

int main(void) {
    char *document;
    Token *jsonTokens;
    Token *childKeys;
    Token token;
    Token JSON;
    document = getJsonLine("C:\\Users\\ilyad\\CLionProjects\\untitled5\\json-test.json");

    JSON = getJSON(document);
    token = getValue(JSON);
    printToken(document,token);
//    childKeys = getChildKeys(JSON);
//    token = getValue(childKeys[3]);
//    printf("%d : %d", token.start, token.end);
//    printf(document);
//    printToken(document, token);
//    printf("\n");
//    childKeys = getChilds(token);
//
//    printf("%d", _msize(childKeys) / sizeof(Token));
//    for (int i = 0; i < _msize(childKeys) / sizeof(Token); ++i) {
//        token = getValue(childKeys[i]);
//        printToken(document, childKeys[i]);
//        printf("\n");
//    }







//    jsonTokens = getJsonTokens(document); //получаем пропарсенный json
//    childKeys = getChildKeys(jsonTokens[10], jsonTokens); //получаем ключи всех детей
//    if (childKeys != NULL) { // проверяем если они есть
//        for (int i = 0; i < _msize(childKeys) / sizeof(Token); ++i) {
//            token = getValue(childKeys[i], jsonTokens);
//
//            printToken(document, token);
//            printf("\n");
//        }
//    }


//    for (int i = 0; i < 25; ++i) {
//        printf("%d : ", jsonTokens[i].type);
//        for (int j = jsonTokens[i].start; j < jsonTokens[i].end; ++j) {
//            printf("%c", document[j]);
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
//                printf("%c", document[v]);
//            }
//            printf(" at [%d;%d]  size = %d\n", childs[i].start, childs[i].end, childs[i].size);
//        }
//    }
//    printToken(document, childs[0]);
//    getKeys(token);
//    printf(getValueToken("productName",result, jsonTokens, count));

    printf("\n");
    return 0;
}
