#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../backend/support/shared.h"
#include "../../backend/semantic-analysis/abstract-syntax-tree.h"

/**
 * Se definen las acciones a ejecutar sobre cada regla de producción de la
 * gramática. El objetivo de cada acción debe ser el de construir el nodo
 * adecuado que almacene la información requerida en el árbol de sintaxis
 * abstracta (i.e., el AST).
 */

// Programa.
int StartGrammarAction(const int uml);
int UmlGrammarAction(const int body, const int nextUml);
int UmlBodyGrammarAction(const int classDef, const int body);

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

int ClassDefinitionGrammarAction(const int name, const int ext, const int imp, const int body);
int InterfaceDefinitionGrammarAction(const int name, const int ext, const int body);
int ExtendsGrammarAction(const int type);
int ImplementsGrammarAction(const int commaSeparatedTypenames);
int ClassBodyGrammarAction(const int content, const int next);
int ClassBodyContentGrammarAction(const int acc, const int elem);
int ClassConstructorGrammarAction(const int name, const int params, const int inlineCode);
int ClassElementGrammarAction(const int mods, const int type, const int name, const int params, const int inlineCode);
int InterfaceBodyGrammarAction(const int content, const int next);
int InterfaceBodyContentGrammarAction(const int acc, const int mods, const int type, const int name, const int params, const int inlineCode);

/* -V-------------------------------------- Methods --------------------------------------V- */

int ClassMethodGrammarAction(const int mods, const int type, const int name, const int params);
int InterfaceMethodGrammarAction(const int mods, const int type, const int name, const int params);
int MethodParamsGrammarAction(const int paramList);
int ParameterGrammarAction(const int type, const int name);
int ParameterListGrammarAction(const int type, const int name, const int next);

/* -V-------------------------------------- Variables --------------------------------------V- */

int ClassVariableGrammarAction(const int mods, const int type, const int name);
int InterfaceVariableGrammarAction(const int mods, const int type, const int name);
int DefaultGrammarAction();
int PrivateGrammarAction();
int ProtectedGrammarAction();
int PublicGrammarAction();
int AbstractGrammarAction();
int StaticGrammarAction();
int FinalGrammarAction();

/* -V-------------------------------------- Misc --------------------------------------V- */

int SymbolnameGrammarAction(const char* symbol);
int TypenameGrammarAction(const char* name);
int GenericTypenameGrammarAction(const char* name, const int genericType);
int CommaSeparatedTypenameGrammarAction(const int type);
int CommaSeparatedTypenamesGrammarAction(const int type, const int next);

/* -V-------------------------------------- Inlines --------------------------------------V- */

int InlineContentGrammarAction(const char* content);
int InlineContentsGrammarAction(const char* content, const int next);
int InlineCodeGrammarAction(const int codeContents);
int InlineCommentGrammarAction(const int commentContents);

#endif
