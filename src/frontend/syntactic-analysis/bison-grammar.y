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
%type <string> interfaceMethodOrVariableModifiers
%type <string> maybeMethodParams
%type <string> methodParams
%type <string> parameterList
%type <string> abstractModifier
%type <string> staticModifier
%type <string> classMethodOrVariableModifiers
%type <string> accessModifier
%type <string> symbolName
%type <string> typeName
%type <string> commaSeparatedTypenames
%type <string> inlineContents
%type <string> inlineCode
%type <string> inlineComment
%type <string> maybeInlineCode

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

umlBody: umlBodyContent[bodyContent] umlBody[nextBody]				{ $$ = UmlBodyGrammarAction($bodyContent, $nextBody); }
	| /* lambda */													{ $$ = 0; }
	;

umlBodyContent: ENDLINE												{ $$ = 0; }
	| classDefinition												{ $$ = $1; }
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

classBodyContent: accessModifier[acc] classElement[elem] maybeInlineCode[code]
																	{ $$ = ClassBodyContentGrammarAction($acc, $elem, $code); }
	| inlineComment[comment]										{ $$ = InlineCommentGrammarAction($comment); }
	| /* lambda */													{ $$ = 0; }
	;

classElement: symbolName[name] methodParams[params]					{ $$ = ClassConstructorGrammarAction($name, $params); }
	| typeName[type] symbolName[name] maybeMethodParams[params]		{ $$ = ClassElementGrammarAction(0, $type, $name, $params); }
	| classMethodOrVariableModifiers[mods] typeName[type] symbolName[name] maybeMethodParams[params]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params); }
	;

abstractClassDefinition: ABSTRACT CLASS typeName[name] extends[ext] implements[imp] OPEN_BLOCK abstractClassBody[body] CLOSE_BLOCK 
																	{ $$ = ClassDefinitionGrammarAction($name, $ext, $imp, $body); }
	;

abstractClassBody: abstractClassBodyContent[content] ENDLINE abstractClassBody[next]
																	{ $$ = ClassBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

abstractClassBodyContent: accessModifier[acc] classElement[elem] maybeInlineCode[code]
																	{ $$ = ClassBodyContentGrammarAction($acc, $elem, $code); }
	| accessModifier[acc] abstractClassElement[elem]				{ $$ = ClassBodyContentGrammarAction($acc, $elem, 0); }
	| inlineComment[comment]										{ $$ = InlineCommentGrammarAction($comment); }
    | /* lambda */													{ $$ = 0; }
	;

abstractClassElement: abstractModifier[mods] typeName[type] symbolName[name] methodParams[params]
																	{ $$ = ClassElementGrammarAction($mods, $type, $name, $params); }
	;

interfaceBody: interfaceBodyContent[content] ENDLINE interfaceBody[next]
																	{ $$ = InterfaceBodyGrammarAction($content, $next); }
	| /* lambda */													{ $$ = 0; }
	;

interfaceBodyContent: abstractModifier[mods] typeName[type] symbolName[name] methodParams[params]
																	{ $$ = InterfaceBodyContentGrammarAction($mods, $type, $name, $params, 0); }
	| interfaceMethodOrVariableModifiers[mods] typeName[type] symbolName[name] maybeMethodParams[params] maybeInlineCode[code]
																	{ $$ = InterfaceBodyContentGrammarAction($mods, $type, $name, $params, $code); }
	| inlineComment[comment]										{ $$ = InlineCommentGrammarAction($comment); }
	| /* lambda */													{ $$ = 0; }
	;

interfaceMethodOrVariableModifiers: staticModifier					{ $$ = $1; }
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

classMethodOrVariableModifiers: staticModifier						{ $$ = $1; }
	| FINAL															{ $$ = FinalGrammarAction(); }
	| staticModifier FINAL											{ $$ = $1 + FinalGrammarAction(); }
	| FINAL staticModifier											{ $$ = FinalGrammarAction() + $1; }
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

%%
