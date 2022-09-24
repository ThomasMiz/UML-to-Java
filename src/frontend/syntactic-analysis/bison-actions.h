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

int UmlGrammarAction(const int body, const int uml);

int UmlBodyGrammarAction(const int classDef, const int body);

int ClassDefinitionGrammarAction(const int classModifiers, const char* className, const int classBody);

#endif
