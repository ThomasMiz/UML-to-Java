%{

#include "bison-actions.h"

%}

// Tipos de dato utilizados en las variables semánticas ($$, $1, $2, etc.).
%union {
	// No-terminales (backend).
	/*
	Program program;
	Expression expression;
	Factor factor;
	Constant constant;
	...
	*/

	// No-terminales (frontend).
	int string;

	// Terminales.
	token token;
	char symbolName[33];
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

%token <symbolName> INLINE_CONTENT
%token <symbolName> SYMBOLNAME

// Tipos de dato para los no-terminales generados desde Bison.
%type <string> start
%type <string> uml
%type <string> umlBody
%type <string> umlBodyContent
%type <string> classDefinition
%type <string> interfaceDefinition
%type <string> extends
%type <string> implements
%type <string> classBody
%type <string> classBodyContent
%type <string> classElement
%type <string> abstractClassDefinition
%type <string> abstractClassBody
%type <string> abstractClassBodyContent
%type <string> abstractClassElement
%type <string> interfaceBody
%type <string> interfaceBodyContent
%type <string> maybeMethodParams
%type <string> methodParams
%type <string> parameterList
%type <string> abstractModifier
%type <string> staticModifier
%type <string> staticXorFinalModifier
%type <string> staticAndFinalModifiers
%type <string> accessModifier
%type <string> maybeAccessModifier
%type <string> symbolName
%type <string> typeName
%type <string> commaSeparatedTypenames
%type <string> inlineContents
%type <string> inlineCode
%type <string> inlineComment
%type <string> maybeInlineCode
%type <string> inlineImport
%type <string> inlineImportList

// Reglas de asociatividad y precedencia (de menor a mayor).
// ((las borré todos))

// El símbolo inicial de la gramatica.
%start start

%%

start: uml															{ $$ = StartGrammarAction($1); }
	;

uml: STARTUML umlBody[body] ENDUML uml[nextUml]						{ $$ = UmlGrammarAction($body, $nextUml); }
	| /* lambda */													{ $$ = 0; }
	;

umlBody: umlBodyContent[bodyContent] ENDLINE umlBody[nextBody]		{ $$ = UmlBodyGrammarAction($bodyContent, $nextBody); }
	| ENDLINE umlBody[nextBody]										{ $$ = $nextBody; }
	| /* lambda */													{ $$ = 0; }
	;

umlBodyContent: classDefinition										{ $$ = $1; }
	| abstractClassDefinition										{ $$ = $1; }
	| interfaceDefinition											{ $$ = $1; }
	;

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

classDefinition: CLASS typeName[name] extends[ext] implements[imp] OPEN_BLOCK classBody[body] CLOSE_BLOCK
																	{ $$ = ClassDefinitionGrammarAction($name, $ext, $imp, $body); }
	;

interfaceDefinition: INTERFACE typeName[name] extends[ext] OPEN_BLOCK interfaceBody[body] CLOSE_BLOCK
									  								{ $$ = InterfaceDefinitionGrammarAction($name, $ext, $body); }
	;

extends: EXTENDS typeName[type]										{ $$ = ExtendsGrammarAction($type); }
	| /* lambda */													{ $$ = 0; }
	;

implements: IMPLEMENTS commaSeparatedTypenames[types]				{ $$ = ImplementsGrammarAction($types); } 
	| /* lambda */													{ $$ = 0; }
	;

classBody: classBodyContent[content] ENDLINE classBody[next]		{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

classBodyContent: maybeAccessModifier[acc] classElement[elem]		{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
	| inlineComment													{ $$ = $1; }
	| /* lambda */													{ $$ = 0; }
	;

classElement: symbolName[name] methodParams[params]	maybeInlineCode[code]
																	{ $$ = ClassConstructorGrammarAction($name, $params, $code); }
	| typeName[type] symbolName[name] maybeMethodParams[params]	maybeInlineCode[code]
																	{ $$ = ClassElementGrammarAction(0, $type, $name, $params, $code); }
	| staticXorFinalModifier[mods] typeName[type] symbolName[name] methodParams[params] inlineCode[code]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params, $code); }
	| staticXorFinalModifier[mods] typeName[type] symbolName[name] maybeInlineCode[code]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, 0, $code); }
	| staticAndFinalModifiers[mods] typeName[type] symbolName[name] maybeMethodParams[params] inlineCode[code]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params, $code); }
	;

abstractClassDefinition: ABSTRACT CLASS typeName[name] extends[ext] implements[imp] OPEN_BLOCK abstractClassBody[body] CLOSE_BLOCK 
																	{ $$ = ClassDefinitionGrammarAction($name, $ext, $imp, $body); }
	;

abstractClassBody: abstractClassBodyContent[content] ENDLINE abstractClassBody[next]
																	{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

abstractClassBodyContent: maybeAccessModifier[acc] classElement[elem]
																	{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
	| maybeAccessModifier[acc] abstractClassElement[elem]			{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
	| inlineComment													{ $$ = $1; }
    | /* lambda */													{ $$ = 0; }
	;

abstractClassElement: abstractModifier[mods] typeName[type] symbolName[name] methodParams[params]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params, 0); }
	;

interfaceBody: interfaceBodyContent[content] ENDLINE interfaceBody[next]
																	{ $$ = InterfaceBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

interfaceBodyContent: abstractModifier[mods] typeName[type] symbolName[name] methodParams[params]
																	{ $$ = InterfaceBodyContentGrammarAction(0, $mods, $type, $name, $params, 0); }
	| staticModifier[mods] typeName[type] symbolName[name] maybeInlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(0, $mods, $type, $name, 0, $code); }
	| staticModifier[mods] typeName[type] symbolName[name] methodParams[params] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(0, $mods, $type, $name, $params, $code); }
	| accessModifier[acc] staticModifier[mods] typeName[type] symbolName[name] maybeInlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction($acc, $mods, $type, $name, 0, $code); }
	| accessModifier[acc] staticModifier[mods] typeName[type] symbolName[name] methodParams[params] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction($acc, $mods, $type, $name, $params, $code); }
	| staticAndFinalModifiers[mods] typeName[type] symbolName[name] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(0, $mods, $type, $name, 0, $code); }
	| accessModifier[acc] staticAndFinalModifiers[mods] typeName[type] symbolName[name] inlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction($acc, $mods, $type, $name, 0, $code); }
	| typeName[type] symbolName[name] methodParams[params] maybeInlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction(0, 0, $type, $name, $params, $code); }
	| typeName[type] symbolName[name] inlineCode[code]				{ $$ = InterfaceBodyContentGrammarAction(0, 0, $type, $name, 0, $code); }
	| inlineComment													{ $$ = $1; }
	| /* lambda */													{ $$ = 0; }
	;

maybeMethodParams: methodParams										{ $$ = $1; }
	| /* lambda */													{ $$ = 0; }
	;

methodParams: OPEN_PARENTHESIS parameterList[paramList] CLOSE_PARENTHESIS
																	{ $$ = MethodParamsGrammarAction($paramList); }
	;

parameterList: typeName[type] symbolName[name]						{ $$ = ParameterGrammarAction($type, $name); }
	| typeName[type] symbolName[name] COMMA parameterList[next]		{ $$ = ParameterListGrammarAction($type, $name, $next); }
	| /* lambda */													{ $$ = 0; }
	;

abstractModifier: OPEN_BLOCK ABSTRACT CLOSE_BLOCK					{ $$ = AbstractGrammarAction(); }
	;

staticModifier: OPEN_BLOCK STATIC CLOSE_BLOCK						{ $$ = StaticGrammarAction(); }
	;

staticXorFinalModifier: staticModifier								{ $$ = $1; }
	| FINAL															{ $$ = FinalGrammarAction(); }
	;

staticAndFinalModifiers: staticModifier FINAL						{ $$ = $1 + FinalGrammarAction(); }
	| FINAL staticModifier											{ $$ = FinalGrammarAction() + $1; }
	;

/* -V-------------------------------------- General --------------------------------------V- */

accessModifier: DEFAULT 											{ $$ = DefaultGrammarAction(); }
	| PRIVATE 														{ $$ = PrivateGrammarAction(); }
	| PROTECTED 													{ $$ = ProtectedGrammarAction(); }
	| PUBLIC														{ $$ = PublicGrammarAction(); }
	;

maybeAccessModifier: accessModifier									{ $$ = $1; }
	| /* lambda */													{ $$ = 0; }
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

/* -V-------------------------------------- Inlines --------------------------------------V- */

inlineContents: INLINE_CONTENT[content]								{ $$ = InlineContentGrammarAction($content); }
	| INLINE_CONTENT[content] inlineContents[next]					{ $$ = InlineContentsGrammarAction($content, $next); }
	;

inlineCode: INLINE_CODE inlineContents[codeContents]				{ $$ = InlineCodeGrammarAction($codeContents); }
	;

inlineComment: INLINE_COMMENT inlineContents[commentContents]		{ $$ = InlineCommentGrammarAction($commentContents); }
	;

maybeInlineCode: inlineCode											{ $$ = $1; }
	| /* lambda */													{ $$ = 0; }
	;

inlineImport: INLINE_IMPORT inlineContents[importContents]			{ $$ = InlineImportGrammarAction($importContents); }
	;

inlineImportList: inlineImport[import] ENDLINE inlineImportList[next] { $$ = InlineImportListGrammarAction($import, $next); }
	| /* lambda */													{ $$ = 0; }
	;

%%
