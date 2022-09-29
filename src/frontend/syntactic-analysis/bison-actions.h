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

int ClassDefinitionGrammarAction(const int mods, const char* name, const int ext, const int imp, const int body);
int InterfaceDefinitionGrammarAction(const int mods, const char* name, const int ext, const int body);
int ExtendsGrammarAction(const char* type);
int ImplementsGrammarAction(const int commaSeparatedTypenames);
int CommaSeparatedTypenameGrammarAction(const char* type);
int CommaSeparatedTypenamesGrammarAction(const char* type, const int next);
int ClassBodyGrammarAction(const int content, const int next);
int InterfaceBodyGrammarAction(const int content, const int next);

/* -V-------------------------------------- Methods --------------------------------------V- */

int ClassMethodGrammarAction(const int mods, const char* type, const char* name, const int params);
int InterfaceMethodGrammarAction(const int mods, const char* type, const char* name, const int params);
int ParameterGrammarAction(const char* type, const char* name);
int ParameterListGrammarAction(const char* type, const char* name, const int next);

/* -V-------------------------------------- Variables --------------------------------------V- */

int ClassVariableGrammarAction(const int mods, const char* type, const char* name);
int InterfaceVariableGrammarAction(const int mods, const char* type, const char* name);
int DefaultGrammarAction();
int PrivateGrammarAction();
int ProtectedGrammarAction();
int PublicGrammarAction();
int AbstractGrammarAction();
int StaticGrammarAction();
int FinalGrammarAction();

#endif
