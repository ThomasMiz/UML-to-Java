#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../backend/support/shared.h"
#include "../syntactic-analysis/bison-parser.h"

/**
 * Se definen las acciones a ejecutar sobre cada patrón hallado mediante el
 * analizador léxico Flex. Este analizador solo puede identificar
 * construcciones regulares, ya que utiliza un autómata finito determinístico
 * (a.k.a. DFA), como mecanismo de escaneo y reconocimiento.
 *
 * Debería existir una acción por cada patrón disponible en Flex (es decir,
 * por cada regla).
 *
 * Solo aquellas acciones que tengan interés en conocer el largo del lexema
 * emplean el parámetro adicional "length". Esto es útil cuando se desea
 * asignar memoria dinámica, y por lo tanto, se debe saber exactamente el
 * largo de aquello que se va a almacenar.
 */

token StartUMLPatternAction();
token EndUMLPatternAction();
token EndlinePatternAction();

/* ---------------------------------------------------- */

token ClassPatternAction();
token InterfacePatternAction();
token ExtendsPatternAction();
token ImplementsPatternAction();

/* ---------------------------------------------------- */

token StaticPatternAction();
token AbstractPatternAction();
token FinalPatternAction();

/* ---------------------------------------------------- */

token DefaultPatternAction();
token PrivatePatternAction();
token ProtectedPatternAction();
token PublicPatternAction();

/* ---------------------------------------------------- */

token SymbolnamePatternAction(const char* lexeme, const int length);
token CommaPatternAction();

/* ---------------------------------------------------- */

token OpenBlockPatternAction();
token CloseBlockPatternAction();
token OpenParenthesisPatternAction();
token CloseParenthesisPatternAction();
token OpenGenericPatternAction();
token CloseGenericPatternAction();

/* ---------------------------------------------------- */
void StartMultilineCommentPatternAction();
void StartSinglelineCommentPatternAction();
token EndSinglelineCommentPatternAction();
void EndMultilineCommentPatternAction();
void EndMultilineInlinePatternAction();
token EndSinglelineInlinePatternAction();

token StartInlineCodePatternAction();
token StartInlineCommentPatternAction();
token StartInlineImportPatternAction();
token InlineContentPatternAction(const char* lexeme, const int length);

#endif