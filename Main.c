#include <stdio.h>
#include "Parser.h"

static void startDocument(){
    printf("starDocument");
    printf("\n");
}
static void endDocument(){
    printf("endDocument");
    printf("\n");
}
static void startElement(TokenType tokenType){
    printf("startElement");
    printf("\n");
}
static void endElement(TokenType tokenType){
    printf("endElement");
    printf("\n");
}
static void characters(Token token){
    printf("characters >>> ");
    printToken(token);
    printf("\n");
}

int main(void) {
    /**
     * SAX ParserJSON
     **/

    Token JSON = getJSON("C:\\Users\\ilyad\\CLionProjects\\ParserJSON\\testsJSON\\pass1.json", startDocument,
                         endDocument, startElement, endElement, characters);


    return 0;
}


