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

token EndlinePatternAction() {
	LogDebug("EndlinePatternAction.");
	yylval.token = ENDLINE;
	return ENDLINE;
}

/* ---------------------------------------------------- */

token ClassPatternAction() {
	LogDebug("ClassPatternAction.");
	yylval.token = CLASS;
	return CLASS;
}

token InterfacePatternAction() {
	LogDebug("InterfacePatternAction.");
	yylval.token = INTERFACE;
	return INTERFACE;
}

token ExtendsPatternAction() {
	LogDebug("ExtendsPatternAction.");
	yylval.token = EXTENDS;
	return EXTENDS;
}

token ImplementsPatternAction() {
	LogDebug("ImplementsPatternAction.");
	yylval.token = IMPLEMENTS;
	return IMPLEMENTS;
}

/* ---------------------------------------------------- */

token StaticPatternAction() {
	LogDebug("StaticPatternAction.");
	yylval.token = STATIC;
	return STATIC;
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

/* ---------------------------------------------------- */

token DefaultPatternAction() {
	LogDebug("DefaultPatternAction.");
	yylval.token = DEFAULT;
	return DEFAULT;
}

token PrivatePatternAction() {
	LogDebug("PrivatePatternAction.");
	yylval.token = PRIVATE;
	return PRIVATE;
}

token ProtectedPatternAction() {
	LogDebug("ProtectedPatternAction.");
	yylval.token = PROTECTED;
	return PROTECTED;
}

token PublicPatternAction() {
	LogDebug("PublicPatternAction.");
	yylval.token = PUBLIC;
	return PUBLIC;
}

/* ---------------------------------------------------- */

token SymbolnamePatternAction(const char* lexeme, const int length) {
	LogDebug("SymbolnamePatternAction(%s, %d).", lexeme, length);
	strncpy(yylval.symbolName, lexeme, sizeof(yylval.symbolName)-1);
	return SYMBOLNAME;
}

token CommaPatternAction() {
	LogDebug("CommaPatternAction.");
	yylval.token = COMMA;
	return COMMA;
}

/* ---------------------------------------------------- */

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

token OpenParenthesisPatternAction() {
	LogDebug("OpenParenthesisPatternAction.");
	yylval.token = OPEN_PARENTHESIS;
	return OPEN_PARENTHESIS;
}

token CloseParenthesisPatternAction() {
	LogDebug("CloseParenthesisPatternAction.");
	yylval.token = CLOSE_PARENTHESIS;
	return CLOSE_PARENTHESIS;
}

token OpenGenericPatternAction() {
	LogDebug("OpenGenericPatternAction.");
	yylval.token = OPEN_GENERIC;
	return OPEN_GENERIC;
}

token CloseGenericPatternAction() {
	LogDebug("CloseGenericPatternAction.");
	yylval.token = CLOSE_GENERIC;
	return CLOSE_GENERIC;
}

/* ---------------------------------------------------- */

void StartMultilineCommentPatternAction() {
	LogDebug("StartMultilineCommentPatternAction.");
}

void StartSinglelineCommentPatternAction() {
	LogDebug("StartSinglelineCommentPatternAction.");
}

token EndSinglelineCommentPatternAction() {
	LogDebug("EndSinglelineCommentPatternAction.");
	yylval.token = ENDLINE;
	return ENDLINE;
}

void EndMultilineCommentPatternAction() {
	LogDebug("EndMultilineCommentPatternAction.");
}

void EndMultilineInlinePatternAction() {
	LogDebug("EndMultilineInlinePatternAction.");
}

token EndSinglelineInlinePatternAction() {
	LogDebug("EndSinglelineInlinePatternAction.");
	yylval.token = ENDLINE;
	return ENDLINE;
}

/* ---------------------------------------------------- */

token StartInlineCodePatternAction() {
	LogDebug("StartInlineCodePatternAction.");
	yylval.token = INLINE_CODE;
	return INLINE_CODE;
}

token StartInlineCommentPatternAction() {
	LogDebug("StartInlineCommentPatternAction.");
	yylval.token = INLINE_COMMENT;
	return INLINE_COMMENT;
}

token InlineContentPatternAction(const char* lexeme, const int length) {
	LogDebug("InlineContentPatternAction(%s, %d).", lexeme, length);
	yylval.token = INLINE_CONTENT;
	return INLINE_CONTENT;
}