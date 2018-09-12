#include <stdio.h>
#include <stdlib.h>
#include "Parser.h"

/**
 * Require for give string value tokenType
 * */
static char *getStringType(TokenType tokenType) {
    switch (tokenType) {
        case ARRAY:
            return "ARRAY";
        case OBJECT:
            return "OBJECT";
        case NUMBER:
            return "NUMBER";
        case STRING:
            return "STRING";
        case NULL_VALUE:
            return "NULL";
        case TRUE_VALUE:
            return "TRUE";
        case FALSE_VALUE:
            return "FALSE";
        default:
            perror("");
            exit(1);
    }
}
/**
 * Прерываем программу с помощью ввода данных
 * */
static void interrupt() {
    int c = getchar();
    while (c != '\n'){
        c = getchar();
    }
}

/**
 * implements require for callback notify event
 */
static void startDocument() {
    printf("starDocument");
    interrupt();
}

static void endDocument() {
    printf("endDocument");
    interrupt();
}

static void startElement(TokenType tokenType) {
    printf("startElement. Type = ");
    printf("%s", getStringType(tokenType));
    interrupt();
}

static void endElement(TokenType tokenType) {
    getStringType(tokenType);
    printf("endElement. Type = ");
    printf("%s", getStringType(tokenType));
    interrupt();
}

static void characters(Token token) {
    printf("characters >>> ");
    printToken(token);
    interrupt();
}



int main(int argc, char *argv[]) {

    /**
     * SAX ParserJSON
     **/

    if (argc != 2) {
        perror("Does not match the format of the input command. \n For example: \"parser C:\\Users\\ilya\\Desktop\\testForJson\\fail1.json\"");
        return 1;
    }
    getJSON(argv[1], startDocument,
            endDocument, startElement, endElement, characters);
    freeParserJSON();

    return 0;
}


