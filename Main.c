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
 * implements require for callback notify event
 */
static void startDocument() {
    printf("starDocument");
    printf("\n");
}

static void endDocument() {
    printf("endDocument");
    printf("\n");
}

static void startElement(TokenType tokenType) {
    printf("startElement. Type = ");
    printf(getStringType(tokenType));
    printf("\n");
}

static void endElement(TokenType tokenType) {
    getStringType(tokenType);
    printf("endElement. Type = ");
    printf(getStringType(tokenType));
    printf("\n");
}

static void characters(Token token) {
    printf("characters >>> ");
    printToken(token);
    printf("\n");
}

int main(void) {

    /**
     * SAX ParserJSON
     **/

//    if (argc != 2){
//        perror("Does not match the format of the input command. \n For example: \"parser C:\\Users\\ilya\\Desktop\\testForJson\\pass1.json\"");
//        return 1;
//    }
    getJSON("C:\\Users\\ilyad\\CLionProjects\\ParserJSON\\testsJSON\\fail1.json", startDocument,
                         endDocument, startElement, endElement, characters);


    return 0;
}


