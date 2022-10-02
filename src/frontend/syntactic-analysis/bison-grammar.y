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
%type <string> commaSeparatedTypenames
%type <string> classBody
%type <string> interfaceBody
%type <string> classBodyContent
%type <string> interfaceBodyContent
%type <string> classModifiers
%type <string> interfaceModifiers
/* %type <string> classMethod */
/* %type <string> interfaceMethod */
%type <string> parameterList
/* %type <string> classVariable */
/* %type <string> interfaceVariable */
/* %type <string> classVariableModifiers */
/* %type <string> interfaceVariableModifiers */
%type <string> classElementModifiers
/* %type <string> classMethodModifiers */
%type <string> interfaceElementModifiers
%type <string> accessModifier
%type <symbolName> symbolName
%type <symbolName> typeName


// Reglas de asociatividad y precedencia (de menor a mayor).
// ((las borré todos))
%right ENDLINE
%right IMPLEMENTS
%right EXTENDS
%right STATIC FINAL
%right ABSTRACT
%right COMMA
%right DEFAULT PRIVATE PUBLIC PROTECTED
%right CLASS INTERFACE
%right OPEN_PARENTHESIS CLOSE_PARENTHESIS
%right STARTUML ENDUML



// El símbolo inicial de la gramatica.
%start start

%%

start: uml															{ $$ = StartGrammarAction($1); }
	;

uml: STARTUML umlBody[body] ENDUML uml[nextUml]						{ $$ = UmlGrammarAction($body, $nextUml); }
	| /* lambda */													{ $$ = 0; }
	;

umlBody: umlBodyContent[bodyContent] umlBody[nextBody]				{ $$ = UmlBodyGrammarAction($bodyContent, $nextBody); }
	| umlBodyContent												{ $$ = 0; }
	;

umlBodyContent: ENDLINE												{ $$ = 0; }
	| classDefinition												{ $$ = $1; }
	| interfaceDefinition											{ $$ = $1; }
	;

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

classDefinition: classModifiers[mods] CLASS typeName[name] extends[ext] implements[imp]
	OPEN_BLOCK classBody[body] CLOSE_BLOCK							{ $$ = ClassDefinitionGrammarAction($mods, $name, $ext, $imp, $body); }
	;

interfaceDefinition: interfaceModifiers[mods] INTERFACE typeName[name] extends[ext] OPEN_BLOCK
	interfaceBody[body] CLOSE_BLOCK  								{ $$ = 0;}
	| interfaceModifiers[mods] INTERFACE typeName[name] extends[ext] OPEN_BLOCK CLOSE_BLOCK
																	{ $$ = 0;}
	;

extends: EXTENDS typeName[type]										{ $$ = ExtendsGrammarAction($type); }
	| /* lambda */													{ $$ = 0; }
	;

implements: IMPLEMENTS commaSeparatedTypenames[types]				{ $$ = ImplementsGrammarAction($types); } 
	| /* lambda */													{ $$ = 0; }
;	

commaSeparatedTypenames: typeName[type]								{ $$ = CommaSeparatedTypenameGrammarAction($type); }
	| typeName[type] COMMA commaSeparatedTypenames[next]			{ $$ = CommaSeparatedTypenamesGrammarAction($type, $next); }
	;

classBody: classBodyContent[content] classBody[next]				{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

interfaceBody: interfaceBodyContent[content] interfaceBody[next]	{ $$ = InterfaceBodyGrammarAction($content, $next); }
	| interfaceBodyContent											{ $$ = 0; }
	;

classBodyContent: accessModifier classElementModifiers[mods] typeName[type] symbolName[name] maybeParams
	ENDLINE															{ $$ = $1; }
	| ENDLINE														{ $$ = 0; }
	;

	maybeParams: OPEN_PARENTHESIS parameterList[params] CLOSE_PARENTHESIS
	| /* lambda */
	;

interfaceBodyContent: interfaceElementModifiers[mods] typeName 
	symbolName maybeParams ENDLINE									{ $$ = $1; }
	| ENDLINE														{ $$ = 0; }
	;

classModifiers: accessModifier ABSTRACT								{ $$ = $1 + AbstractGrammarAction(); }
	| accessModifier FINAL											{ $$ = $1 + FinalGrammarAction(); }
	| accessModifier												{ $$ = $1; }
	;

interfaceModifiers: accessModifier ABSTRACT							{ $$ = $1 + AbstractGrammarAction(); }
	| accessModifier												{ $$ = $1; }
	;

/* -V-------------------------------------- Methods --------------------------------------V- */

/* classMethod: classMethodModifiers[mods] typeName[type] symbolName[name] OPEN_PARENTHESIS
	parameterList[params] CLOSE_PARENTHESIS ENDLINE
																	{ $$ = ClassMethodGrammarAction($mods, $type, $name, $params); }
	; */

/* interfaceMethod: interfaceMethodModifiers[mods] typeName[type] symbolName[name] OPEN_PARENTHESIS
	parameterList[params] CLOSE_PARENTHESIS ENDLINE
																	{ $$ = InterfaceMethodGrammarAction($mods, $type, $name, $params); }
	; */

parameterList: typeName[type] symbolName[name]						{ $$ = ParameterGrammarAction($type, $name); }
	| typeName[type] symbolName[name] COMMA parameterList[next]		{ $$ = ParameterListGrammarAction($type, $name, $next); }
	| /* lambda */													{ $$ = 0; }
	;

/* -V-------------------------------------- Variables --------------------------------------V- */

/* classVariable: classVariableModifiers[mods] typeName[type] symbolName[name] ENDLINE
																	{ $$ = ClassVariableGrammarAction($mods, $type, $name); }
	;

interfaceVariable: interfaceVariableModifiers[mods] typeName[type] symbolName[name] ENDLINE
																	{ $$ = InterfaceVariableGrammarAction($mods, $type, $name); }
	; */

/* classVariableModifiers: FINAL						{ $$ = $1 + FinalGrammarAction(); }
	| STATIC											{ $$ = $1 + StaticGrammarAction(); }
	| 												{ $$ = 0; }
	;  */

/* // Solamente podemos tener variables de tipo static en una interfaz 
interfaceVariableModifiers: STATIC 									{ $$ = StaticGrammarAction(); }
	| STATIC FINAL 													{ $$ = StaticGrammarAction() + FinalGrammarAction(); }
	| FINAL STATIC 													{ $$ = FinalGrammarAction() + StaticGrammarAction(); }
	; */

classElementModifiers: FINAL 										{ $$ = $1 + FinalGrammarAction(); }
	| ABSTRACT 														{ $$ = $1 + AbstractGrammarAction(); }
	| STATIC 														{ $$ = $1 + StaticGrammarAction(); }
	| /* lambda */ 													{ $$ = 0; }
	;

interfaceElementModifiers: PUBLIC 									{ $$ = PublicGrammarAction(); }
	| ABSTRACT 														{ $$ = AbstractGrammarAction(); }
	| STATIC 														{ $$ = StaticGrammarAction(); }
	| PUBLIC ABSTRACT 												{ $$ = PublicGrammarAction() + AbstractGrammarAction(); }
	| PUBLIC STATIC 												{ $$ = PublicGrammarAction() + StaticGrammarAction(); }
	| STATIC FINAL 													{ $$ = StaticGrammarAction() + FinalGrammarAction(); }
	| FINAL STATIC 													{ $$ = FinalGrammarAction() + StaticGrammarAction(); }
	| /* lambda */ 													{ $$ = 0; }
	;

/* -V-------------------------------------- General --------------------------------------V- */

accessModifier: DEFAULT 											{ $$ = DefaultGrammarAction(); }
	| PRIVATE 														{ $$ = PrivateGrammarAction(); }
	| PROTECTED 													{ $$ = ProtectedGrammarAction(); }
	| PUBLIC														{ $$ = PublicGrammarAction(); }
	| /* lambda */													{ $$ = 0; }
	;

symbolName: SYMBOLNAME
	;

typeName: SYMBOLNAME
	;

%%
