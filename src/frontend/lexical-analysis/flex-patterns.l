%{

#include "flex-actions.h"

%}

/* Obliga a crear una regla explícita para manejar lexemas desconocidos. */
%option nodefault

/* Escanea un único archivo y se detiene. */
%option noyywrap

/* Dispone una variable global "yylineno" con el número de línea actual. */
%option yylineno

/* Contextos de Flex. */
%x UML
%x CLASS

/* Patrones reutilizables. */
accessModifier  [-+#~]
symbolName      [a-zA-Z][a-zA-Z0-9_]*
endline			[\n]
whitespace		[ \f\n\r\t\v]

%%
"@startuml"									{ BEGIN(UML); return StartUMLPatternAction(); }
"@enduml"                                   { return EndUMLPatternAction(); }
.                                           { ; }
{endline}                                   { ; }

<UML>{endline}                              { ; }
<UML>{whitespace}                           { ; }
<UML>"@enduml"								{ token t = EndUMLPatternAction(); BEGIN(INITIAL); return t;  }

<UML>"class"                                { return ClassPatternAction(); }

<UML>"abstract"                             { return AbstractPatternAction(); }
<UML>"final"                                { return FinalPatternAction(); }

<UML>{symbolName}                           { return SymbolnamePatternAction(yytext, yyleng); }

<UML>"{"                                    { return OpenBlockPatternAction(); }
<UML>"}"                                    { return CloseBlockPatternAction(); }
%%
