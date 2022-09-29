#include "../../backend/domain-specific/calculator.h"
#include "../../backend/support/logger.h"
#include "bison-actions.h"
#include <stdio.h>
#include <string.h>

/**
 * Implementación de "bison-grammar.h".
 */

/**
* Esta función se ejecuta cada vez que se emite un error de sintaxis.
*/
void yyerror(const char * string) {
	LogError("Mensaje: '%s' debido a '%s' (linea %d).", string, yytext, yylineno);
	LogError("En ASCII es:");
	LogErrorRaw("\t");
	const int length = strlen(yytext);
	for (int i = 0; i < length; ++i) {
		LogErrorRaw("[%d]", yytext[i]);
	}
	LogErrorRaw("\n\n");
}

/**
* Esta acción se corresponde con el no-terminal que representa el símbolo
* inicial de la gramática, y por ende, es el último en ser ejecutado, lo que
* indica que efectivamente el programa de entrada se pudo generar con esta
* gramática, o lo que es lo mismo, que el programa pertenece al lenguaje.
*/
int StartGrammarAction(const int uml) {
	LogDebug("\tStartGrammarAction(%d)", uml);
	/*
	* "state" es una variable global que almacena el estado del compilador,
	* cuyo campo "succeed" indica si la compilación fue o no exitosa, la cual
	* es utilizada en la función "main".
	*/
	state.succeed = true;
	/*
	* Por otro lado, "result" contiene el resultado de aplicar el análisis
	* sintáctico mediante Bison, y almacenar el nood raíz del AST construido
	* en esta variable. Para el ejemplo de la calculadora, no hay AST porque
	* la expresión se computa on-the-fly, y es la razón por la cual esta
	* variable es un simple entero, en lugar de un nodo.
	*/
	state.result = uml;
	return state.result;
}

int UmlGrammarAction(const int body, const int nextUml) {
	LogDebug("\tUmlGrammarAction(%d, %d)", body, nextUml);
	return body + nextUml;
}

int UmlBodyGrammarAction(const int classDef, const int body) {
	LogDebug("\tUmlBodyGrammarAction(%d, %d)", classDef, body);
	return classDef + body;
}

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

int ClassDefinitionGrammarAction(const int mods, const char* name, const int ext, const int imp, const int body) {
	LogDebug("\tClassDefinitionGrammarAction(%d, %s, %d, %d, %d)", mods, name, ext, imp, body);
	return mods + 1 + ext + imp + body;
}

int InterfaceDefinitionGrammarAction(const int mods, const char* name, const int ext, const int body) {
	LogDebug("\tInterfaceDefinitionGrammarAction(%d, %s, %d, %d)", mods, name, ext, body);
	return mods + 1 + ext + body;
}

int ExtendsGrammarAction(const char* type) {
	LogDebug("\tExtendsGrammarAction(%s)", type);
	return 1;
}

int ImplementsGrammarAction(const int commaSeparatedTypenames) {
	LogDebug("\tImplementsGrammarAction(%d)", commaSeparatedTypenames);
	return commaSeparatedTypenames;
}

int CommaSeparatedTypenameGrammarAction(const char* type) {
	LogDebug("\tCommaSeparatedTypenameGrammarAction(%s)", type);
	return 1;
}

int CommaSeparatedTypenamesGrammarAction(const char* type, const int next) {
	LogDebug("\tCommaSeparatedTypenamesGrammarAction(%s, %d)", type, next);
	return 1 + next;
}

int ClassBodyGrammarAction(const int content, const int next) {
	LogDebug("\tClassBodyGrammarAction(%d, %d)", content, next);
	return content + next;
}

int InterfaceBodyGrammarAction(const int content, const int next) {
	LogDebug("\tInterfaceBodyGrammarAction(%d, %d)", content, next);
	return content + next;
}

/* -V-------------------------------------- Methods --------------------------------------V- */

int ClassMethodGrammarAction(const int mods, const char* type, const char* name, const int params) {
	LogDebug("\tClassMethodGrammarAction(%d, %s, %s, %d)", mods, type, name, params);
	return mods + 1 + 1 + params;
}

int InterfaceMethodGrammarAction(const int mods, const char* type, const char* name, const int params) {
	LogDebug("\tInterfaceMethodGrammarAction(%d, %s, %s, %d)", mods, type, name, params);
	return mods + 1 + 1 + params;
}

int ParameterGrammarAction(const char* type, const char* name) {
	LogDebug("\tParameterGrammarAction(%s, %s)", type, name);
	return 1 + 1;
}

int ParameterListGrammarAction(const char* type, const char* name, const int next) {
	LogDebug("\tParameterListGrammarAction(%s, %s, %d)", type, name, next);
	return 1 + 1 + next;
}

/* -V-------------------------------------- Variables --------------------------------------V- */

int ClassVariableGrammarAction(const int mods, const char* type, const char* name) {
	LogDebug("\tClassVariableGrammarAction(%d, %s, %s)", mods, type, name);
	return mods + 1 + 1;
}

int InterfaceVariableGrammarAction(const int mods, const char* type, const char* name) {
	LogDebug("\tInterfaceVariableGrammarAction(%d, %s, %s)", mods, type, name);
	return mods + 1 + 1;
}

int DefaultGrammarAction() {
	LogDebug("\tDefaultGrammarAction()");
	return 1;
}

int PrivateGrammarAction() {
	LogDebug("\tPrivateGrammarAction()");
	return 1;
}

int ProtectedGrammarAction() {
	LogDebug("\tProtectedGrammarAction()");
	return 1;
}

int PublicGrammarAction() {
	LogDebug("\tPublicGrammarAction()");
	return 1;
}

int AbstractGrammarAction() {
	LogDebug("\tAbstractGrammarAction()");
	return 1;
}

int StaticGrammarAction() {
	LogDebug("\tStaticGrammarAction()");
	return 1;
}

int FinalGrammarAction() {
	LogDebug("\tFinalGrammarAction()");
	return 1;
}