#include <stdio.h>
#include "Parser.h"

int main(void) {
    /**
     * SAX ParserJSON
     **/
    Token JSON = getJSON("C:\\Users\\ilyad\\CLionProjects\\ParserJSON\\testsJSON\\pass1.json");
    Token * tokens = getJsonTokens();
    for (int i = 0; i < tokens->size; ++i) {
//        printToken(tokens[i]);
        printf("\t\t\t");
        printf("size = %d", tokens[i].size);
        printf("\t\t\t");
        printf("type = %d", tokens[i].type);
//        printf("\t\t\t");
        printf("\n");
    }
//    printToken(JSON);
    return 0;
}
