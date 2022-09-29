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

/* Patrones reutilizables. */
symbolName      [a-zA-Z][a-zA-Z0-9_]*
endline			[\n]
whitespace		[ \f\n\r\t\v]


%%
"@startuml"									{ BEGIN(UML); return StartUMLPatternAction(); }
"@enduml"                                   { return EndUMLPatternAction(); }
.                                           { ; }
{endline}                                   { ; }

<UML>{endline}                              { return EndlinePatternAction(); }
<UML>{whitespace}                           { ; }
<UML>"@enduml"								{ token t = EndUMLPatternAction(); BEGIN(INITIAL); return t;  }

<UML>"class"                                { return ClassPatternAction(); }
<UML>"interface"                            { return InterfacePatternAction(); }
<UML>"extends"                              { return ExtendsPatternAction(); }
<UML>"implements"                           { return ImplementsPatternAction(); }

<UML>"static"                               { return StaticPatternAction(); }
<UML>"abstract"                             { return AbstractPatternAction(); }
<UML>"final"                                { return FinalPatternAction(); }

<UML>{symbolName}                           { return SymbolnamePatternAction(yytext, yyleng); }
<UML>","                                    { return CommaPatternAction(); }

<UML>"~"                                    { return DefaultPatternAction(); }
<UML>"-"                                    { return PrivatePatternAction(); }
<UML>"#"                                    { return ProtectedPatternAction(); }
<UML>"+"                                    { return PublicPatternAction(); }

<UML>"{"                                    { return OpenBlockPatternAction(); }
<UML>"}"                                    { return CloseBlockPatternAction(); }
<UML>"("                                    { return OpenParenthesisPatternAction(); }
<UML>")"                                    { return CloseParenthesisPatternAction(); }
%%
