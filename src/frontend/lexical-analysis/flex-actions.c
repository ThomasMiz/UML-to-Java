#include "../../backend/support/logger.h"
#include "flex-actions.h"
#include <stdlib.h>
#include <string.h>

/**
 * Implementación de "flex-actions.h".
 *
 * Cada función debe realizar 2 operaciones básicas para lograr el streaming
 * de tokens hacia Bison:
 *
 *	1) Computar los atributos del token y almacenarlos donde sea conveniente
 *		(en la tabla de símbolos, en "yylval", o en ambos).
 *	2) Retornar el token que identifica el terminal identificado.
 *
 * Bison utilizará el token retornado en el punto (2) para matchear dicho
 * terminal en la gramática. Por otro lado, el valor almacenado en "yylval" es
 * el que Bison proveerá como valor semántico al realizar una reducción
 * (mediante $1, $2, $3, etc.).
 */

token StartUMLPatternAction() {
	LogDebug("StartUMLPatternAction.");
	yylval.token = STARTUML;
	return STARTUML;
}

token EndUMLPatternAction() {
	LogDebug("EndUMLPatternAction.");
	yylval.token = ENDUML;
	return ENDUML;
}

token ClassPatternAction() {
	LogDebug("ClassPatternAction.");
	yylval.token = CLASS;
	return CLASS;
}

token AbstractPatternAction() {
	LogDebug("AbstractPatternAction.");
	yylval.token = ABSTRACT;
	return ABSTRACT;
}

token FinalPatternAction() {
	LogDebug("FinalPatternAction.");
	yylval.token = FINAL;
	return FINAL;
}

token SymbolnamePatternAction(const char* lexeme, const int length) {
	LogDebug("SymbolnamePatternAction.");
	strncpy(yylval.symbolName, lexeme, sizeof(yylval.symbolName)-1);
	return SYMBOLNAME;
}

token OpenBlockPatternAction() {
	LogDebug("OpenBlockPatternAction.");
	yylval.token = OPEN_BLOCK;
	return OPEN_BLOCK;
}

token CloseBlockPatternAction() {
	LogDebug("CloseBlockPatternAction.");
	yylval.token = CLOSE_BLOCK;
	return CLOSE_BLOCK;
}