#ifndef _BISON_ACTIONS_H_
#define _BISON_ACTIONS_H_

#include "../../backend/support/shared.h"
#include "../../backend/semantic-analysis/abstract-syntax-tree.h"
#include "../../utils/buffer.h"
/**
 * Se definen las acciones a ejecutar sobre cada regla de producción de la
 * gramática. El objetivo de cada acción debe ser el de construir el nodo
 * adecuado que almacene la información requerida en el árbol de sintaxis
 * abstracta (i.e., el AST).
 */

int StartGrammarAction(const TUml* uml);
TUml* UmlGrammarAction(const TUmlBody* body, const TUml* next);
const TUmlBody* UmlBodyGrammarAction(const TClassDefinition* classDefinition, const TUmlBody* next);

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

const TClassDefinition* ClassDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TCommaSeparatedTypenames* implements, const TInlineImportList* imports, const TClassBody* body);
const TClassDefinition* AbstractClassDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TCommaSeparatedTypenames* implements, const TInlineImportList* imports, const TClassBody* body);
const TClassDefinition* InterfaceDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TInlineImportList* imports, const TClassBody* body);
const TTypeName* ExtendsGrammarAction(const TTypeName* type);
const TCommaSeparatedTypenames* ImplementsGrammarAction(const TCommaSeparatedTypenames* commaSeparatedTypenames);
const TClassBody* ClassBodyGrammarAction(TClassBody* body, const TClassBody* next);
TClassBody* ClassBodyContentGrammarAction(const TAccessModifiers accessMods, TClassElement* element);
TClassBody* ClassInlineCommentGrammarAction(const TInlineContent* content);
TClassElement* ClassConstructorGrammarAction(const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode);
TClassElement* ClassElementGrammarAction(const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode);
const TClassBody* InterfaceBodyGrammarAction(TClassBody* body, const TClassBody* next);
const TClassBody* InterfaceBodyContentGrammarAction(const TAccessModifiers accessMods, const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode);

/* -V-------------------------------------- Methods --------------------------------------V- */

const TMethodParameterList* MethodParamsGrammarAction(const TParameterList* paramList);
const TParameterList* ParameterGrammarAction(const TTypeName* type, const char* name);
const TParameterList* ParameterListGrammarAction(const TTypeName* type, const char* name, const TParameterList* next);

/* -V-------------------------------------- Modifiers --------------------------------------V- */

TAccessModifiers DefaultGrammarAction();
TAccessModifiers PrivateGrammarAction();
TAccessModifiers ProtectedGrammarAction();
TAccessModifiers PublicGrammarAction();
TElementModifiers AbstractGrammarAction();
TElementModifiers StaticGrammarAction();
TElementModifiers FinalGrammarAction();

/* -V-------------------------------------- Misc --------------------------------------V- */

const char* SymbolnameGrammarAction(const char* symbol);
const TTypeName* TypenameGrammarAction(const char* name);
const TTypeName* GenericTypenameGrammarAction(const char* name, const TCommaSeparatedTypenames* genericType);
const TCommaSeparatedTypenames* CommaSeparatedTypenameGrammarAction(const TTypeName* type);
const TCommaSeparatedTypenames* CommaSeparatedTypenamesGrammarAction(const TTypeName* type, const TCommaSeparatedTypenames* next);
/* -V-------------------------------------- Inlines --------------------------------------V- */

const TInlineContent* InlineContentGrammarAction(const char* content);
const TInlineContent* InlineContentsGrammarAction(const char* content, const TInlineContent* next);
const TInlineContent* InlineCodeGrammarAction(const TInlineContent* content);
const TInlineContent* InlineCommentGrammarAction(const TInlineContent* content);
const TInlineContent* InlineImportGrammarAction(const TInlineContent* content);
const TInlineImportList* InlineImportListGrammarAction(const TInlineContent* content, const TInlineImportList* next);

void writeTTypeName(const bufferADT buffer, const TTypeName* name);

#endif