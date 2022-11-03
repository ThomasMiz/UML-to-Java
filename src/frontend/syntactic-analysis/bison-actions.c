#include "../../backend/domain-specific/calculator.h"
#include "../../backend/support/logger.h"
#include "bison-actions.h"
#include <stdio.h>
#include <stdlib.h>
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
int StartGrammarAction(const TUml* uml) {
    LogDebug("\tStartGrammarAction(%lu)", uml);
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
    state.result = 420;
    state.program = uml;
    return state.result;
}

const TUml* UmlGrammarAction(const TUmlBody* body, const TUml* next) {
    LogDebug("\tUmlGrammarAction(%lu, %lu)", body, next);
    TUml* node = malloc(sizeof(TUml));
    node->body = body;
    node->next = next;
    return node;
}

const TUmlBody* UmlBodyGrammarAction(const TClassDefinition* classDefinition, const TUmlBody* next) {
    LogDebug("\tUmlBodyGrammarAction(%lu, %lu)", classDefinition, next);
    TUmlBody* node = malloc(sizeof(TUmlBody));
    node->bodyContent = classDefinition;
    node->next = next;
    return node;
}

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

const TClassDefinition* ClassDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TCommaSeparatedTypenames* implements, const TInlineImportList* imports, const TClassBody* body) {
    LogDebug("\tClassDefinitionGrammarAction(%s, %lu, %lu, %lu, %lu)", name, extends, implements, imports, body);
    TClassDefinition* node = malloc(sizeof(TClassDefinition));
    node->name = name;
    node->type = CTYPE_CLASS;
    node->extends = extends;
    node->implements = implements;
    node->imports = imports;
    node->body = body;
    return node;
}

const TClassDefinition* InterfaceDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TInlineImportList* imports, const TClassBody* body) {
    LogDebug("\tInterfaceDefinitionGrammarAction(%s, %lu, %lu, %lu)", name, extends, imports, body);
    TClassDefinition* node = malloc(sizeof(TClassDefinition));
    node->name = name;
    node->type = CTYPE_INTERFACE;
    node->extends = extends;
    node->implements = NULL;
    node->imports = imports;
    node->body = body;
    return node;
}

const TTypeName* ExtendsGrammarAction(const TTypeName* type) {
    LogDebug("\tExtendsGrammarAction(%lu)", type);
    return type;
}

const TCommaSeparatedTypenames* ImplementsGrammarAction(const TCommaSeparatedTypenames* commaSeparatedTypenames) {
    LogDebug("\tImplementsGrammarAction(%lu)", commaSeparatedTypenames);
    return commaSeparatedTypenames;
}

const TClassBody* ClassBodyGrammarAction(TClassBody* body, const TClassBody* next) {
    LogDebug("\tClassBodyGrammarAction(%lu, %lu)", body, next);
    body->next = next;
    return body;
}

TClassBody* ClassBodyContentGrammarAction(const TAccessModifiers accessMods, TClassElement* element) {
    LogDebug("\tClassBodyContentGrammarAction(%d, %lu)", accessMods, element);
    element->accessModifiers = accessMods;
    TClassBody* node = malloc(sizeof(TClassBody));
    node->comment = NULL;
    node->element = element;
    node->next = NULL;
    return node;
}

TClassBody* ClassInlineCommentGrammarAction(const TInlineContent* content) {
    LogDebug("\tClassInlineCommentGrammarAction(%lu)", content);
    TClassBody* node = malloc(sizeof(TClassBody));
    node->comment = content;
    node->element = NULL;
    node->next = NULL;
    return node;
}

TClassElement* ClassConstructorGrammarAction(const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("\tClassConstructorGrammarAction(%d, %d, %d)", name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = AMODS_NONE;
    node->elementModifiers = AMODS_NONE;
    node->typeName = NULL;
    node->symbolName = name;
    node->parameterList = params;
    node->inlineCode = inlineCode;
    return node;
}

TClassElement* ClassElementGrammarAction(const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("\tClassElementGrammarAction(%d, %lu, %lu, %lu, %lu)", elementMods, type, name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = AMODS_NONE;
    node->elementModifiers = elementMods;
    node->typeName = type;
    node->symbolName = name;
    node->parameterList = params;
    node->inlineCode = inlineCode;
    return node;
}

const TClassBody* InterfaceBodyGrammarAction(TClassBody* body, const TClassBody* next) {
    LogDebug("\tInterfaceBodyGrammarAction(%lu, %lu)", body, next);
    body->next = next;
    return body;
}

TClassBody* InterfaceBodyContentGrammarAction(const TAccessModifiers accessMods, const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("\tInterfaceBodyContentGrammarAction(%d, %d, %lu, %lu, %lu, %lu)", accessMods, elementMods, type, name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = accessMods;
    node->elementModifiers = elementMods;
    node->typeName = type;
    node->symbolName = name;
    node->parameterList = params;
    node->inlineCode = inlineCode;

    TClassBody* body = malloc(sizeof(TClassBody));
    body->comment = NULL;
    body->element = node;
    body->next = NULL;

    return body;
}

/* -V-------------------------------------- Methods --------------------------------------V- */

const TMethodParameterList* MethodParamsGrammarAction(const TParameterList* paramList) {
    LogDebug("\tMethodParamsGrammarAction(%lu)", paramList);
    TMethodParameterList* node = malloc(sizeof(TMethodParameterList));
    node->parameterList = paramList;
    return node;
}

const TParameterList* ParameterGrammarAction(const TTypeName* type, const char* name) {
    LogDebug("\tParameterGrammarAction(%lu, %lu)", type, name);
    TParameterList* node = malloc(sizeof(TParameterList));
    node->typeName = type;
    node->symbolName = name;
    node->next = NULL;
    return node;
}

const TParameterList* ParameterListGrammarAction(const TTypeName* type, const char* name, const TParameterList* next) {
    LogDebug("\tParameterListGrammarAction(%lu, %lu, %lu)", type, name, next);
    TParameterList* node = malloc(sizeof(TParameterList));
    node->typeName = type;
    node->symbolName = name;
    node->next = next;
    return node;
}

/* -V-------------------------------------- Modifiers --------------------------------------V- */

TAccessModifiers DefaultGrammarAction() {
    LogDebug("\tDefaultGrammarAction()");
    return AMODS_DEFAULT;
}

TAccessModifiers PrivateGrammarAction() {
    LogDebug("\tPrivateGrammarAction()");
    return AMODS_PRIVATE;
}

TAccessModifiers ProtectedGrammarAction() {
    LogDebug("\tProtectedGrammarAction()");
    return AMODS_PROTECTED;
}

TAccessModifiers PublicGrammarAction() {
    LogDebug("\tPublicGrammarAction()");
    return AMODS_PUBLIC;
}

TElementModifiers AbstractGrammarAction() {
    LogDebug("\tAbstractGrammarAction()");
    return EMODS_ABSTRACT;
}

TElementModifiers StaticGrammarAction() {
    LogDebug("\tStaticGrammarAction()");
    return EMODS_STATIC;
}

TElementModifiers FinalGrammarAction() {
    LogDebug("\tFinalGrammarAction()");
    return EMODS_FINAL;
}

/* -V-------------------------------------- Misc --------------------------------------V- */

const char* SymbolnameGrammarAction(const char* symbol) {
    LogDebug("\tSymbolnameGrammarAction(%s)", symbol);
    return symbol;
}

const TTypeName* TypenameGrammarAction(const char* name) {
    LogDebug("\tTypenameGrammarAction(%s)", name);
    TTypeName* node = malloc(sizeof(TTypeName));
    node->symbolName = name;
    node->genericType = NULL;
    return node;
}

const TTypeName* GenericTypenameGrammarAction(const char* name, const TCommaSeparatedTypenames* genericType) {
    LogDebug("\tGenericTypenameGrammarAction(%s, %lu)", name, genericType);
    TTypeName* node = malloc(sizeof(TTypeName));
    node->symbolName = name;
    node->genericType = genericType;
    return node;
}

const TCommaSeparatedTypenames* CommaSeparatedTypenameGrammarAction(const TTypeName* type) {
    LogDebug("\tCommaSeparatedTypenameGrammarAction(%lu)", type);
    TCommaSeparatedTypenames* node = malloc(sizeof(TCommaSeparatedTypenames));
    node->typeName = type;
    node->next = NULL;
    return node;
}

const TCommaSeparatedTypenames* CommaSeparatedTypenamesGrammarAction(const TTypeName* type, const TCommaSeparatedTypenames* next) {
    LogDebug("\tCommaSeparatedTypenamesGrammarAction(%lu, %lu)", type, next);
    TCommaSeparatedTypenames* node = malloc(sizeof(TCommaSeparatedTypenames));
    node->typeName = type;
    node->next = next;
    return node;
}

/* -V-------------------------------------- Inlines --------------------------------------V- */

const TInlineContent* InlineContentGrammarAction(const char* content) {
    LogDebug("\tInlineContentGrammarAction(%s)", content);
    TInlineContent* node = malloc(sizeof(TInlineContent));
    node->content = content;
    node->next = NULL;
    return node;
}

const TInlineContent* InlineContentsGrammarAction(const char* content, const TInlineContent* next) {
    LogDebug("\tInlineContentsGrammarAction(%s, %lu)", content, next);
    TInlineContent* node = malloc(sizeof(TInlineContent));
    node->content = content;
    node->next = next;
    return node;
}

const TInlineContent* InlineCodeGrammarAction(const TInlineContent* content) {
    LogDebug("\tInlineCodeGrammarAction(%lu)", content);
    return content;
}

const TInlineContent* InlineCommentGrammarAction(const TInlineContent* content) {
    LogDebug("\tInlineCommentGrammarAction(%lu)", content);
    return content;
}

const TInlineContent* InlineImportGrammarAction(const TInlineContent* content) {
    LogDebug("\tInlineImportGrammarAction(%lu)", content);
    return content;
}

const TInlineImportList* InlineImportListGrammarAction(const TInlineContent* content, const TInlineImportList* next) {
    LogDebug("\tInlineImportListGrammarAction(%lu, %lu)", content, next);
    TInlineImportList* node = malloc(sizeof(TInlineImportList));
    node->import = content;
    node->next = next;
    return node;
}
