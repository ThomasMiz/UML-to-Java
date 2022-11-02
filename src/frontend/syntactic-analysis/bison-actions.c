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
void yyerror(const char* string) {
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

int ClassDefinitionGrammarAction(const int name, const int ext, const int imp, const int imports, const int body) {
    LogDebug("\tClassDefinitionGrammarAction(%d, %d, %d, %d, %d)", name, ext, imp, imports, body);
    return name + ext + imp + imports + body;
}

int InterfaceDefinitionGrammarAction(const int name, const int ext, const int imports, const int body) {
    LogDebug("\tInterfaceDefinitionGrammarAction(%d, %d, %d, %d)", name, ext, imports, body);
    return name + ext + imports + body;
}

int ExtendsGrammarAction(const int type) {
    LogDebug("\tExtendsGrammarAction(%d)", type);
    return type;
}

int ImplementsGrammarAction(const int commaSeparatedTypenames) {
    LogDebug("\tImplementsGrammarAction(%d)", commaSeparatedTypenames);
    return commaSeparatedTypenames;
}

int ClassBodyGrammarAction(const int content, const int next) {
    LogDebug("\tClassBodyGrammarAction(%d, %d)", content, next);
    return content + next;
}

int ClassBodyContentGrammarAction(const int acc, const int elem) {
    LogDebug("\tClassBodyContentGrammarAction(%d, %d)", acc, elem);
    return acc + elem;
}

int ClassConstructorGrammarAction(const int name, const int params, const int inlineCode) {
    LogDebug("\tClassConstructorGrammarAction(%d, %d, %d)", name, params, inlineCode);
    return name + params + inlineCode;
}

int ClassElementGrammarAction(const int mods, const int type, const int name, const int params, const int inlineCode) {
    LogDebug("\tClassElementGrammarAction(%d, %d, %d, %d, %d)", mods, type, name, params, inlineCode);
    return mods + type + name + params + inlineCode;
}

int InterfaceBodyGrammarAction(const int content, const int next) {
    LogDebug("\tInterfaceBodyGrammarAction(%d, %d)", content, next);
    return content + next;
}

int InterfaceBodyContentGrammarAction(const int acc, const int mods, const int type, const int name, const int params, const int inlineCode) {
    LogDebug("\tInterfaceBodyContentGrammarAction(%d, %d, %d, %d, %d, %d)", acc, mods, type, name, params, inlineCode);
    return acc + mods + type + name + params + inlineCode;
}

/* -V-------------------------------------- Methods --------------------------------------V- */

int ClassMethodGrammarAction(const int mods, const int type, const int name, const int params) {
    LogDebug("\tClassMethodGrammarAction(%d, %d, %d, %d)", mods, type, name, params);
    return mods + type + name + params;
}

int InterfaceMethodGrammarAction(const int mods, const int type, const int name, const int params) {
    LogDebug("\tInterfaceMethodGrammarAction(%d, %d, %d, %d)", mods, type, name, params);
    return mods + type + name + params;
}

int MethodParamsGrammarAction(const int paramList) {
    LogDebug("\tMethodParamsGrammarAction(%d)", paramList);
    return paramList;
}

int ParameterGrammarAction(const int type, const int name) {
    LogDebug("\tParameterGrammarAction(%d, %d)", type, name);
    return type + name;
}

int ParameterListGrammarAction(const int type, const int name, const int next) {
    LogDebug("\tParameterListGrammarAction(%d, %d, %d)", type, name, next);
    return type + name + next;
}

/* -V-------------------------------------- Variables --------------------------------------V- */

int ClassVariableGrammarAction(const int mods, const int type, const int name) {
    LogDebug("\tClassVariableGrammarAction(%d, %d, %d)", mods, type, name);
    return mods + type + name;
}

int InterfaceVariableGrammarAction(const int mods, const int type, const int name) {
    LogDebug("\tInterfaceVariableGrammarAction(%d, %d, %d)", mods, type, name);
    return mods + type + name;
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

/* -V-------------------------------------- Misc --------------------------------------V- */

int SymbolnameGrammarAction(const char* symbol) {
    LogDebug("\tSymbolnameGrammarAction(%s)", symbol);
    return 1;
}

int TypenameGrammarAction(const char* name) {
    LogDebug("\tTypenameGrammarAction(%s)", name);
    return 1;
}

int GenericTypenameGrammarAction(const char* name, const int genericType) {
    LogDebug("\tGenericTypenameGrammarAction(%s, %d)", name, genericType);
    return 1 + genericType;
}

int CommaSeparatedTypenameGrammarAction(const int type) {
    LogDebug("\tCommaSeparatedTypenameGrammarAction(%d)", type);
    return type;
}

int CommaSeparatedTypenamesGrammarAction(const int type, const int next) {
    LogDebug("\tCommaSeparatedTypenamesGrammarAction(%d, %d)", type, next);
    return type + next;
}

/* -V-------------------------------------- Inlines --------------------------------------V- */

int InlineContentGrammarAction(const char* content) {
    LogDebug("\tInlineContentGrammarAction(%s)", content);
    return 1;
}

int InlineContentsGrammarAction(const char* content, const int next) {
    LogDebug("\tInlineContentsGrammarAction(%s, %d)", content, next);
    return 1 + next;
}

int InlineCodeGrammarAction(const int content) {
    LogDebug("\tInlineCodeGrammarAction(%d)", content);
    return content;
}

int InlineCommentGrammarAction(const int content) {
    LogDebug("\tInlineCommentGrammarAction(%d)", content);
    return content;
}

int InlineImportGrammarAction(const int content) {
    LogDebug("\tInlineCommentGrammarAction(%d)", content);
    return content;
}

int InlineImportListGrammarAction(const int content, const int next) {
    LogDebug("\tInlineCommentGrammarAction(%d, %d)", content, next);
    return content + next;
}
