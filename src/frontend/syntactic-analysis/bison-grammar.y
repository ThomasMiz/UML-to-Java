%{

#include "bison-actions.h"
#include "../../backend/support/logger.h"

%}

// Tipos de dato utilizados en las variables semánticas ($$, $1, $2, etc.).
%union {
	// No-terminales (backend).
	const TInlineContent* inlineContent;
	const TInlineImportList* inlineImportList;
	const TCommaSeparatedTypenames* commaSeparatedTypenames;
	const TTypeName* typeName;
	TElementModifiers elementModifiers;
	TAccessModifiers accessModifiers;
	const TParameterList* parameterList;
	const TMethodParameterList* methodParameterList;
	const TClassElement* classElement;
	TClassElement* classElementIncomplete;
	const TClassBody* classBody;
	TClassBody* classBodyIncomplete;
	const TClassType* classType;
	const TClassDefinition* classDefinition;
	const TUmlBody* umlBody;
	const TUml* uml;

	// No-terminales (frontend).
	// int string; // No puedo borrar esta línea, es preciosa
	int resultCode;

	// Terminales.
	token token;
	const char* string;
}

// IDs y tipos de los tokens terminales generados desde Flex.
%token <token> STARTUML
%token <token> ENDUML
%token <token> ENDLINE

%token <token> CLASS
%token <token> INTERFACE
%token <token> EXTENDS
%token <token> IMPLEMENTS

%token <token> STATIC
%token <token> ABSTRACT
%token <token> FINAL

%token <token> COMMA

%token <token> DEFAULT
%token <token> PRIVATE
%token <token> PROTECTED
%token <token> PUBLIC

%token <token> OPEN_BLOCK
%token <token> CLOSE_BLOCK
%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_GENERIC
%token <token> CLOSE_GENERIC

%token <token> INLINE_CODE
%token <token> INLINE_COMMENT
%token <token> INLINE_IMPORT

%token <string> INLINE_CONTENT
%token <string> SYMBOLNAME

// Tipos de dato para los no-terminales generados desde Bison.
%type <resultCode> start
%type <uml> uml
%type <umlBody> umlBody
%type <classDefinition> umlBodyContent
%type <classDefinition> classDefinition
%type <classDefinition> interfaceDefinition
%type <typeName> extends
%type <commaSeparatedTypenames> implements
%type <classBody> classBody
%type <classBodyIncomplete> classBodyContent
%type <classElementIncomplete> classElement
%type <classDefinition> abstractClassDefinition
%type <classBody> abstractClassBody
%type <classBodyIncomplete> abstractClassBodyContent
%type <classElementIncomplete> abstractClassElement
%type <classBody> interfaceBody
%type <classBodyIncomplete> interfaceBodyContent
%type <methodParameterList> maybeMethodParams
%type <methodParameterList> methodParams
%type <parameterList> parameterList
%type <elementModifiers> abstractModifier
%type <elementModifiers> staticModifier
%type <elementModifiers> staticXorFinalModifier
%type <elementModifiers> staticAndFinalModifiers
%type <accessModifiers> accessModifier
%type <accessModifiers> maybeAccessModifier
%type <string> symbolName
%type <typeName> typeName
%type <commaSeparatedTypenames> commaSeparatedTypenames
%type <inlineContent> inlineContents
%type <inlineContent> inlineCode
%type <inlineContent> inlineComment
%type <inlineContent> maybeInlineCode
%type <inlineContent> inlineImport
%type <inlineImportList> inlineImportList

// Reglas de asociatividad y precedencia (de menor a mayor).
// ((las borré todos))

// El símbolo inicial de la gramatica.
%start start

%%

start: uml															{ $$ = StartGrammarAction($1); }
	;

uml: STARTUML umlBody[body] ENDUML uml[nextUml]						{ $$ = UmlGrammarAction($body, $nextUml); }
	| /* lambda */													{ $$ = NULL; }
	;

umlBody: umlBodyContent[bodyContent] ENDLINE umlBody[nextBody]		{ $$ = UmlBodyGrammarAction($bodyContent, $nextBody); }
	| ENDLINE umlBody[nextBody]										{ $$ = $nextBody; }
	| /* lambda */													{ $$ = NULL; }
	;

umlBodyContent: classDefinition										{ $$ = $1; }
	| abstractClassDefinition										{ $$ = $1; }
	| interfaceDefinition											{ $$ = $1; }
	;

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

classDefinition: CLASS typeName[name] extends[ext] implements[imp] OPEN_BLOCK maybeEndlines inlineImportList[imports] classBody[body] CLOSE_BLOCK
																	{ $$ = ClassDefinitionGrammarAction($name, $ext, $imp, $imports, $body); }
	;

interfaceDefinition: INTERFACE typeName[name] extends[ext] OPEN_BLOCK maybeEndlines inlineImportList[imports] interfaceBody[body] CLOSE_BLOCK
																	{ $$ = InterfaceDefinitionGrammarAction($name, $ext, $imports, $body); }
	;

extends: EXTENDS typeName[type]										{ $$ = ExtendsGrammarAction($type); }
	| /* lambda */													{ $$ = NULL; }
	;

implements: IMPLEMENTS commaSeparatedTypenames[types]				{ $$ = ImplementsGrammarAction($types); } 
	| /* lambda */													{ $$ = NULL; }
	;

classBody: classBodyContent[content] endlines classBody[next]		{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = NULL; }
	;

classBodyContent: maybeAccessModifier[acc] classElement[elem]		{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
	| inlineComment[comment]										{ $$ = ClassInlineCommentGrammarAction($comment); }
	;

classElement: symbolName[name] methodParams[params]	maybeInlineCode[code]
																	{ $$ = ClassConstructorGrammarAction($name, $params, $code); }
	| typeName[type] symbolName[name] maybeMethodParams[params]	maybeInlineCode[code]
																	{ $$ = ClassElementGrammarAction(EMODS_NONE, $type, $name, $params, $code); }
	| staticXorFinalModifier[mods] typeName[type] symbolName[name] methodParams[params] inlineCode[code]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params, $code); }
	| staticXorFinalModifier[mods] typeName[type] symbolName[name] maybeInlineCode[code]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, NULL, $code); }
	| staticAndFinalModifiers[mods] typeName[type] symbolName[name] maybeMethodParams[params] inlineCode[code]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params, $code); }
	;

abstractClassDefinition: ABSTRACT CLASS typeName[name] extends[ext] implements[imp] OPEN_BLOCK maybeEndlines inlineImportList[imports] abstractClassBody[body] CLOSE_BLOCK 
																	{ $$ = ClassDefinitionGrammarAction($name, $ext, $imp, $imports, $body); }
	;

abstractClassBody: abstractClassBodyContent[content] endlines abstractClassBody[next]
																	{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = NULL; }
	;

abstractClassBodyContent: maybeAccessModifier[acc] classElement[elem]
																	{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
	| maybeAccessModifier[acc] abstractClassElement[elem]			{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
	| inlineComment[comment]										{ $$ = ClassInlineCommentGrammarAction($comment); }
	;

abstractClassElement: abstractModifier[mods] typeName[type] symbolName[name] methodParams[params]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params, NULL); }
	;

interfaceBody: interfaceBodyContent[content] endlines interfaceBody[next]
																	{ $$ = InterfaceBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = NULL; }
	;

interfaceBodyContent: abstractModifier[mods] typeName[type] symbolName[name] methodParams[params]
																	{ $$ = InterfaceBodyContentGrammarAction(EMODS_NONE, $mods, $type, $name, $params, NULL); }
	| staticModifier[mods] typeName[type] symbolName[name] maybeInlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(EMODS_NONE, $mods, $type, $name, NULL, $code); }
	| staticModifier[mods] typeName[type] symbolName[name] methodParams[params] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(EMODS_NONE, $mods, $type, $name, $params, $code); }
	| accessModifier[acc] staticModifier[mods] typeName[type] symbolName[name] maybeInlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction($acc, $mods, $type, $name, NULL, $code); }
	| accessModifier[acc] staticModifier[mods] typeName[type] symbolName[name] methodParams[params] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction($acc, $mods, $type, $name, $params, $code); }
	| staticAndFinalModifiers[mods] typeName[type] symbolName[name] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(EMODS_NONE, $mods, $type, $name, NULL, $code); }
	| accessModifier[acc] staticAndFinalModifiers[mods] typeName[type] symbolName[name] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction($acc, $mods, $type, $name, NULL, $code); }
	| typeName[type] symbolName[name] methodParams[params] maybeInlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(EMODS_NONE, AMODS_NONE, $type, $name, $params, $code); }
	| typeName[type] symbolName[name] inlineCode[code]				{ $$ = InterfaceBodyContentGrammarAction(EMODS_NONE, AMODS_NONE, $type, $name, NULL, $code); }
	| inlineComment[comment]										{ $$ = ClassInlineCommentGrammarAction($comment); }
	;

maybeMethodParams: methodParams										{ $$ = $1; }
	| /* lambda */													{ $$ = NULL; }
	;

methodParams: OPEN_PARENTHESIS parameterList[paramList] CLOSE_PARENTHESIS
																	{ $$ = MethodParamsGrammarAction($paramList); }
	;

parameterList: typeName[type] symbolName[name]						{ $$ = ParameterGrammarAction($type, $name); }
	| typeName[type] symbolName[name] COMMA parameterList[next]		{ $$ = ParameterListGrammarAction($type, $name, $next); }
	| /* lambda */													{ $$ = NULL; }
	;

abstractModifier: OPEN_BLOCK ABSTRACT CLOSE_BLOCK					{ $$ = AbstractGrammarAction(); }
	;

staticModifier: OPEN_BLOCK STATIC CLOSE_BLOCK						{ $$ = StaticGrammarAction(); }
	;

staticXorFinalModifier: staticModifier								{ $$ = $1; }
	| FINAL															{ $$ = EMODS_FINAL;}
	;

staticAndFinalModifiers: staticModifier FINAL						{ $$ = EMODS_FINAL | EMODS_STATIC; }
	| FINAL staticModifier											{ $$ = EMODS_FINAL | EMODS_STATIC; }
	;

/* -V-------------------------------------- General --------------------------------------V- */

accessModifier: DEFAULT 											{ $$ = DefaultGrammarAction(); }
	| PRIVATE 														{ $$ = PrivateGrammarAction(); }
	| PROTECTED 													{ $$ = ProtectedGrammarAction(); }
	| PUBLIC														{ $$ = PublicGrammarAction(); }
	;

maybeAccessModifier: accessModifier									{ $$ = $1; }
	| /* lambda */													{ $$ = AMODS_NONE; }
	;

symbolName: SYMBOLNAME[symbol]										{ $$ = SymbolnameGrammarAction($symbol); }
	;

typeName: SYMBOLNAME[name] OPEN_GENERIC commaSeparatedTypenames[genericType] CLOSE_GENERIC
																	{ $$ = GenericTypenameGrammarAction($name, $genericType); }
	| SYMBOLNAME[name]												{ $$ = TypenameGrammarAction($name); }
	;

commaSeparatedTypenames: typeName[type] COMMA commaSeparatedTypenames[next]
																	{ $$ = CommaSeparatedTypenamesGrammarAction($type, $next); }
	| typeName[type]												{ $$ = CommaSeparatedTypenameGrammarAction($type); }
	;

endlines: ENDLINE endlines | ENDLINE ;

maybeEndlines: ENDLINE maybeEndlines | /* lambda */ ;

/* -V-------------------------------------- Inlines --------------------------------------V- */

inlineContents: INLINE_CONTENT[content]								{ $$ = InlineContentGrammarAction($content); }
	| INLINE_CONTENT[content] inlineContents[next]					{ $$ = InlineContentsGrammarAction($content, $next); }
	;

inlineCode: INLINE_CODE inlineContents[codeContents]				{ $$ = InlineCodeGrammarAction($codeContents); }
	;

inlineComment: INLINE_COMMENT inlineContents[commentContents]		{ $$ = InlineCommentGrammarAction($commentContents); }
	;

maybeInlineCode: inlineCode											{ $$ = $1; }
	| /* lambda */													{ $$ = NULL; }
	;

inlineImport: INLINE_IMPORT inlineContents[importContents]			{ $$ = InlineImportGrammarAction($importContents); }
	;


inlineImportList: inlineImport[import] endlines inlineImportList[next]
																	{ $$ = InlineImportListGrammarAction($import, $next); }
	| /* lambda */													{ $$ = NULL; }
	;

%%
