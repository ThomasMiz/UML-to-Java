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
%x SINGLELINE_COMMENT
%x MULTILINE_COMMENT
%x SINGLELINE_INLINE
%x MULTILINE_INLINE

/* Patrones reutilizables. */
symbolName      [a-zA-Z][a-zA-Z0-9_]*
endline         [\n]
whitespace      [ \f\n\r\t\v]
maybeWhitespace [ \f\n\r\t\v]*
maybeWhitespaceNoNewline [ \f\r\t\v]*
singlelineInlineContent ([^\'\n]|[\|\°\¬\!\"\#\$\%\&\/\(\)\=\?\¡\\\¿\+\*\~\[\]\{\}\^\-\_\:\;\.\,\>\<])*
multilineInlineContent ([^\']|[\|\°\¬\!\"\#\$\%\&\/\(\)\=\?\¡\\\¿\+\*\~\[\]\{\}\^\-\_\:\;\.\,\>\<])*


%%
"@startuml"                                 { BEGIN(UML); return StartUMLPatternAction(); }
"@enduml"                                   { return EndUMLPatternAction(); }
.                                           { ; }
{endline}                                   { ; }

<UML>{endline}                              { return EndlinePatternAction(); }
<UML>{whitespace}                           { ; }
<UML>"@enduml"                              { token t = EndUMLPatternAction(); BEGIN(INITIAL); return t;  }

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
<UML>"<"                                    { return OpenGenericPatternAction(); }
<UML>">"                                    { return CloseGenericPatternAction(); }

<UML>"'"{maybeWhitespaceNoNewline}"code:"   { token t = StartInlineCodePatternAction(); BEGIN(SINGLELINE_INLINE); return t; }
<UML>"'"{maybeWhitespaceNoNewline}"comment:" { token t = StartInlineCommentPatternAction(); BEGIN(SINGLELINE_INLINE); return t; }
<UML>"'"{maybeWhitespaceNoNewline}"import:" { token t = StartInlineImportPatternAction(); BEGIN(SINGLELINE_INLINE); return t; }
<UML>"/'"{maybeWhitespace}"code:"           { token t = StartInlineCodePatternAction(); BEGIN(MULTILINE_INLINE); return t; }
<UML>"/'"{maybeWhitespace}"comment:"        { token t = StartInlineCommentPatternAction(); BEGIN(MULTILINE_INLINE); return t; }
<UML>"/'"                                   { StartMultilineCommentPatternAction(); BEGIN(MULTILINE_COMMENT); }
<UML>"'"                                    { StartSinglelineCommentPatternAction(); BEGIN(SINGLELINE_COMMENT); }

<MULTILINE_COMMENT>"'/"                     { EndMultilineCommentPatternAction(); BEGIN(UML); }
<MULTILINE_COMMENT>{endline}                { ; }
<MULTILINE_COMMENT>.                        { ; }

<SINGLELINE_COMMENT>{endline}               { token t = EndSinglelineCommentPatternAction(); BEGIN(UML); return t; }
<SINGLELINE_COMMENT>.                       { ; }

<MULTILINE_INLINE>"'/"                      { EndMultilineInlinePatternAction(); BEGIN(UML); }
<MULTILINE_INLINE>{multilineInlineContent}  { return InlineContentPatternAction(yytext, yyleng); }
<MULTILINE_INLINE>.                         { return InlineContentPatternAction(yytext, yyleng); }

<SINGLELINE_INLINE>{endline}                { token t = EndSinglelineInlinePatternAction(); BEGIN(UML); return t; }
<SINGLELINE_INLINE>{singlelineInlineContent} { return InlineContentPatternAction(yytext, yyleng); }
<SINGLELINE_INLINE>.                        { return InlineContentPatternAction(yytext, yyleng); }

%% 
