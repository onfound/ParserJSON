//
// Created by ilyad on 08.09.2018.
//

#ifndef PARSERJSON_MAIN_H
#define PARSERJSON_MAIN_H

#include "Parser.h"

static char *getStringType(TokenType tokenType);
static void startDocument();
static void endDocument();
static void startElement(TokenType);
static void endElement(TokenType);
static void characters(Token token);


#endif //PARSERJSON_MAIN_H
