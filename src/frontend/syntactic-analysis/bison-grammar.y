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
	int start;
	int uml;
	int umlBody;
	int classDefinition;
	int classModifier;
	int classBody;

	// Terminales.
	token token;
	char symbolName[33];
}

// IDs y tipos de los tokens terminales generados desde Flex.
%token <token> STARTUML
%token <token> ENDUML

%token <token> CLASS

%token <token> ABSTRACT
%token <token> FINAL

%token <token> OPEN_BLOCK
%token <token> CLOSE_BLOCK

%token <symbolName> SYMBOLNAME

// Tipos de dato para los no-terminales generados desde Bison.
%type <start> start
%type <uml> uml
%type <umlBody> umlBody
%type <classDefinition> classDefinition
%type <classModifier> classModifier
%type <classBody> classBody
%type <symbolName> symbolName

// Reglas de asociatividad y precedencia (de menor a mayor).
// ((las borré todos))

// El símbolo inicial de la gramatica.
%start start

%%

start: uml															{ $$ = StartGrammarAction($1); }

uml: STARTUML umlBody[body] ENDUML uml[nextUml]						{ $$ = UmlGrammarAction($body, $nextUml); }
	| /* lambda */													{ $$ = 0; }

umlBody: classDefinition[classDef] umlBody[body]					{ $$ = UmlBodyGrammarAction($classDef, $body); }
	| /* lambda */													{ $$ = 0; }

classDefinition:
	classModifier[mods] CLASS symbolName[name] OPEN_BLOCK classBody[body] CLOSE_BLOCK
																	{ $$ = ClassDefinitionGrammarAction($mods, $name, $body); }
	;

classModifier: ABSTRACT												{ $$ = 1; }
	| FINAL															{ $$ = 1; }
	| /* lambda */													{ $$ = 0; }
	;

symbolName: SYMBOLNAME
	;

classBody: /* lambda */												{ $$ = 0; }
	;

%%
