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
%type <string> comaSeparatedTypenames
%type <string> classBody
%type <string> interfaceBody
%type <string> classBodyContent
%type <string> interfaceBodyContent
%type <string> classModifiers
%type <string> interfaceModifiers
%type <string> classMethod
%type <string> interfaceMethod
%type <string> parameterList
%type <string> classVariable
%type <string> interfaceVariable
%type <string> classVariableModifiers
%type <string> interfaceVariableModifiers
%type <string> classMethodModifiers
%type <string> interfaceMethodModifiers
%type <string> accessModifier
%type <string> symbolName
%type <string> typeName


// Reglas de asociatividad y precedencia (de menor a mayor).
// ((las borré todos))

// El símbolo inicial de la gramatica.
%start start

%%

start: uml															{ $$ = StartGrammarAction($1); }
	;

uml: STARTUML umlBody[body] ENDUML uml[nextUml]						{ $$ = $1 + $2 + $3 + $4; }
	| /* lambda */													{ $$ = 0; }
	;

umlBody: umlBodyContent[bodyContent] umlBody[body]					{ $$ = $1 + $2; }
	| /* lambda */													{ $$ = 0; }
	;

umlBodyContent: ENDLINE												{ $$ = $1; }
	| classDefinition												{ $$ = $1; }
	| interfaceDefinition											{ $$ = $1; }
	;

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

classDefinition: classModifiers[mods] CLASS typeName[name] extends[ext] implements[imp]
	OPEN_BLOCK classBody[body] CLOSE_BLOCK
																	{ $$ = $1+$2+$3+$4+$5+$6+$7+$8; }
	;

interfaceDefinition: interfaceModifiers[mods] INTERFACE typeName[name] extends[ext] OPEN_BLOCK
	interfaceBody[body] CLOSE_BLOCK
																	{ $$ = $1+$2+$3+$4+$5+$6+$7; }
	;

extends: EXTENDS typeName											{ $$ = $1 + $2; }
	| /* lambda */													{ $$ = 0; }
	;

implements: IMPLEMENTS comaSeparatedTypenames						{ $$ = $1 + $2; } 
	| /* lambda */													{ $$ = 0; }
	;

comaSeparatedTypenames: typeName									{ $$ = $1; }
	| typeName COMMA comaSeparatedTypenames							{ $$ = $1 + $2 + $3; }
	;

classBody: classBodyContent classBody								{ $$ = $1 + $2; }
	| /* lambda */													{ $$ = 0; }
	;

interfaceBody: interfaceBodyContent interfaceBody 					{ $$ = $1 + $2; }
	| /* lambda */													{ $$ = 0; }
	;

classBodyContent: classMethod | classVariable | ENDLINE ;

interfaceBodyContent: interfaceMethod | interfaceVariable | ENDLINE ;

classModifiers: accessModifier ABSTRACT								{ $$ = $1 + $2; }
	| accessModifier FINAL											{ $$ = $1 + $2; }
	| accessModifier												{ $$ = $1; }
	;

interfaceModifiers: accessModifier ABSTRACT							{ $$ = $1 + $2; }
	| accessModifier												{ $$ = $1; }
	;

/* -V-------------------------------------- Methods --------------------------------------V- */

classMethod: classMethodModifiers typeName symbolName OPEN_PARENTHESIS parameterList CLOSE_PARENTHESIS ENDLINE
																	{ $$ = $1+$2+$3+$4+$5+$6; }

interfaceMethod: interfaceMethodModifiers typeName symbolName OPEN_PARENTHESIS parameterList CLOSE_PARENTHESIS ENDLINE
																	{ $$ = $1+$2+$3+$4+$5+$6; }

parameterList: typeName[type] symbolName[name]						{ $$ = $1 + $2; }
	| typeName[type] symbolName[name] COMMA parameterList			{ $$ = $1 + $2 + $3 + $4; }
	| /* lambda */													{ $$ = 0; }
	;

/* -V-------------------------------------- Variables --------------------------------------V- */

classVariable: classVariableModifiers typeName symbolName ENDLINE	{ $$ = $1 + $2 + $3; }
	;

interfaceVariable: interfaceVariableModifiers typeName symbolName ENDLINE
																	{ $$ = $1 + $2 + $3; }
	;

classVariableModifiers: accessModifier FINAL						{ $$ = $1 + $2; }
	| accessModifier STATIC											{ $$ = $1 + $2; }
	| accessModifier												{ $$ = $1; }
	;

// Solamente podemos tener variables de tipo static en una interfaz */
interfaceVariableModifiers: STATIC 									{ $$ = $1; }
	| STATIC FINAL 													{ $$ = $1 + $2; }
	| FINAL STATIC 													{ $$ = $1 + $2; }
	;

classMethodModifiers: accessModifier FINAL 							{ $$ = $1 + $2; }
	| accessModifier ABSTRACT 										{ $$ = $1 + $2; }
	| accessModifier STATIC 										{ $$ = $1 + $2; }
	| accessModifier 												{ $$ = $1; }
	;

interfaceMethodModifiers: PUBLIC 									{ $$ = $1; }
	| ABSTRACT 														{ $$ = $1; }
	| STATIC 														{ $$ = $1; }
	| PUBLIC ABSTRACT 												{ $$ = $1 + $2; }
	| PUBLIC STATIC 												{ $$ = $1 + $2; }
	| /* lambda */ 													{ $$ = 0; }
	;

/* -V-------------------------------------- General --------------------------------------V- */

accessModifier: DEFAULT 											{ $$ = $1; }
	| PRIVATE 														{ $$ = $1; }
	| PROTECTED 													{ $$ = $1; }
	| PUBLIC														{ $$ = $1; }
	| /* lambda */													{ $$ = 0; }
	;

symbolName: SYMBOLNAME 												{ $$ = 1; }
	;

typeName: SYMBOLNAME												{ $$ = 1; }
	;

%%
