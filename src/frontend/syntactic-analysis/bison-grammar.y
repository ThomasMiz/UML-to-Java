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
%type <string> commaSeparatedTypenames
%type <string> classBody
%type <string> classBodyContent
%type <string> classElement
%type <string> interfaceBody
%type <string> interfaceBodyContent
%type <string> interfaceMethodOrVariableModifiers
%type <string> interfaceMethodOnlyModifiers
%type <string> maybeMethodParams
%type <string> methodParams
%type <string> parameterList
%type <string> classAbstractMethodModifier
%type <string> classMethodOrVariableModifiers
%type <string> accessModifier
%type <string> symbolName
%type <string> typeName
%type <string> abstractClassDefinition
%type <string> abstractClassBody
%type <string> abstractClassElement
%type <string> abstractClassBodyContent


// Reglas de asociatividad y precedencia (de menor a mayor).
// ((las borré todos))

// El símbolo inicial de la gramatica.
%start start

%%

start: uml															{ $$ = StartGrammarAction($1); }
	;

uml: STARTUML umlBody[body] ENDUML uml[nextUml]
																	{ $$ = UmlGrammarAction($body, $nextUml); }
	| /* lambda */													{ $$ = 0; }
	;

umlBody: umlBodyContent[bodyContent] umlBody[nextBody]				{ $$ = UmlBodyGrammarAction($bodyContent, $nextBody); }
	| /* lambda */													{ $$ = 0; }
	;

umlBodyContent: ENDLINE												{ $$ = 0; }
	| classDefinition												{ $$ = $1; }
	| abstractClassDefinition										{ $$ = $1; }
	| interfaceDefinition											{ $$ = $1; }
	;

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

classDefinition: CLASS typeName[name] extends[ext] implements[imp]
	OPEN_BLOCK classBody[body] CLOSE_BLOCK							{ $$ = ClassDefinitionGrammarAction($name, $ext, $imp, $body); }
	;

interfaceDefinition: INTERFACE typeName[name] extends[ext] OPEN_BLOCK
	interfaceBody[body] CLOSE_BLOCK  								{ $$ = 0; }
	;

extends: EXTENDS typeName[type]										{ $$ = ExtendsGrammarAction($type); }
	| /* lambda */													{ $$ = 0; }
	;

implements: IMPLEMENTS commaSeparatedTypenames[types]				{ $$ = ImplementsGrammarAction($types); } 
	| /* lambda */													{ $$ = 0; }
	;

classBody: classBodyContent[content] classBody[next]				{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

classBodyContent: accessModifier[acc] classElement[elem] ENDLINE	{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
	| ENDLINE														{ $$ = 0; }
	;

classElement: classMethodOrVariableModifiers[mods] typeName[type] symbolName[name] maybeMethodParams[params]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params); }
	;

abstractClassDefinition: ABSTRACT CLASS typeName[name] extends[ext] implements[imp] OPEN_BLOCK abstractClassBody[body] CLOSE_BLOCK 
																	{ $$ = ClassDefinitionGrammarAction($name, $ext, $imp, $body); }
	;

abstractClassBody: abstractClassBodyContent[content]  abstractClassBody[next]
																	{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

abstractClassBodyContent: accessModifier[acc] abstractClassElement[elem] ENDLINE
																	{ $$ = ClassBodyContentGrammarAction($acc, $elem); }
    | ENDLINE														{ $$ = 0; }
	;

abstractClassElement: classMethodOrVariableModifiers[mods] typeName[type] symbolName[name] maybeMethodParams[params]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params); }
	| classAbstractMethodModifier[mods] typeName[type] symbolName[name] methodParams[params]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params); }
	;

interfaceBody: interfaceBodyContent[content] interfaceBody[next]	{ $$ = InterfaceBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

interfaceBodyContent: interfaceMethodOnlyModifiers[mods] typeName[type] symbolName[name] methodParams[params] ENDLINE
																	{ $$ = InterfaceBodyContentGrammarAction($mods, $type, $name, $params); }
	| interfaceMethodOrVariableModifiers[mods] typeName[type] symbolName[name] maybeMethodParams[params] ENDLINE
																	{ $$ = InterfaceBodyContentGrammarAction($mods, $type, $name, $params); }
	| ENDLINE														{ $$ = 0; }
	;

interfaceMethodOrVariableModifiers: OPEN_BLOCK STATIC CLOSE_BLOCK	{ $$ = StaticGrammarAction(); }
	| /* lambda */													{ $$ = 0; }
	;

interfaceMethodOnlyModifiers: OPEN_BLOCK ABSTRACT CLOSE_BLOCK		{ $$ = AbstractGrammarAction(); }
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

classAbstractMethodModifier: OPEN_BLOCK ABSTRACT CLOSE_BLOCK		{ $$ = AbstractGrammarAction(); }
	;

classMethodOrVariableModifiers: OPEN_BLOCK STATIC CLOSE_BLOCK		{ $$ = FinalGrammarAction(); }
	| FINAL															{ $$ = StaticGrammarAction(); }
	| OPEN_BLOCK STATIC CLOSE_BLOCK FINAL							{ $$ = StaticGrammarAction() + FinalGrammarAction(); }
	| FINAL OPEN_BLOCK STATIC CLOSE_BLOCK							{ $$ = FinalGrammarAction() + StaticGrammarAction(); }
	| /* lambda */													{ $$ = 0; }
	;

/* -V-------------------------------------- General --------------------------------------V- */

accessModifier: DEFAULT 											{ $$ = DefaultGrammarAction(); }
	| PRIVATE 														{ $$ = PrivateGrammarAction(); }
	| PROTECTED 													{ $$ = ProtectedGrammarAction(); }
	| PUBLIC														{ $$ = PublicGrammarAction(); }
	| /* lambda */													{ $$ = 0; }
	;

symbolName: SYMBOLNAME[symbol]										{ $$ = SymbolnameGrammarAction($symbol); }
	;

typeName: SYMBOLNAME[name]											{ $$ = TypenameGrammarAction($name); }
	| SYMBOLNAME[name] OPEN_GENERIC commaSeparatedTypenames[genericType] CLOSE_GENERIC
																	{ $$ = GenericTypenameGrammarAction($name, $genericType); }
	;

commaSeparatedTypenames: typeName[type] COMMA commaSeparatedTypenames[next]
																	{ $$ = CommaSeparatedTypenamesGrammarAction($type, $next); }
	| typeName[type]												{ $$ = CommaSeparatedTypenameGrammarAction($type); }
	;

%%
