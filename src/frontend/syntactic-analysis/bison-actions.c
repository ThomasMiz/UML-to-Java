#include "../../backend/domain-specific/calculator.h"
#include "../../backend/support/logger.h"
#include "bison-actions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils/buffer.h"

/**
 * Implementación de "bison-grammar.h".
 */

/**
 * Esta función se ejecuta cada vez que se emite un error de sintaxis.
 */
void yyerror(const char* string) {
    LogError("Mensaje: '%s' debido a '%s' (linea %d).", string, yytext, yylineno);
    LogError("En ASCII es:");
    LogErrorRaw("\t");
    const int length = strlen(yytext);
    for (int i = 0; i < length; ++i) {
        LogErrorRaw("[%d]", yytext[i]);
    }
    LogErrorRaw("\n\n");
}

void writeTTypeName(bufferADT buffer, TTypeName* name) {
    LogDebug("\twriteTTypeName(%lu)", name);
    if (name == NULL)
        return;

    write_buffer(buffer, name->symbolName);
    writeGenerics(buffer, name->genericType);
}

void writeGenerics(bufferADT buffer, TCommaSeparatedTypenames* generics) {
    LogDebug("\twriteGenerics(%lu)", generics);
    if (generics == NULL)
        return;

    write_buffer(buffer, "<");
    while (generics != NULL) {
        writeTTypeName(buffer, generics->typeName);
        generics = generics->next;
        if (generics != NULL)
            write_buffer(buffer, ", ");
    }
    write_buffer(buffer, ">");
}

void writeTCommaSeparatedTypenames(bufferADT buffer, TCommaSeparatedTypenames* names) {
    LogDebug("\twriteTCommaSeparatedTypenames(%lu)", names);
    while (names != NULL) {
        writeTTypeName(buffer, names->typeName);
        names = names->next;
        if (names != NULL)
            write_buffer(buffer, ", ");
    }
}

void writeTInlineContent(bufferADT buffer, TInlineContent* content) {
    LogDebug("\twriteTInlineContent(%lu)", content);
    while (content != NULL) {
        LogDebug("\tcontent(%lu)", content->content);
        write_buffer(buffer, content->content);
        content = content->next;
    }
}

void writeTInlineImportList(bufferADT buffer, TInlineImportList* imports) {
    LogDebug("\twriteTInlineImportList(%lu)", imports);
    while (imports != NULL) {
        write_buffer(buffer, "import ");
        writeTInlineContent(buffer, imports->import);
        write_buffer(buffer, ";\n");
        imports = imports->next;
    }
}

// AMODS_NONE = 0,
// AMODS_DEFAULT = 1 << 0,
// AMODS_PRIVATE = 1 << 1,
// AMODS_PROTECTED = 1 << 2,
// AMODS_PUBLIC = 1 << 3

void writeTAccessModifiers(bufferADT buffer, TAccessModifiers accessModifiers) {
    if (accessModifiers & AMODS_NONE || accessModifiers & AMODS_DEFAULT) {
        return;
    }

    // uint8_t added = 0;
    // TAccessModifiers added = 0;

    if (accessModifiers & AMODS_PRIVATE) {
        write_buffer(buffer, "private");
        accessModifiers &= ~AMODS_PRIVATE;
    }

    else if (accessModifiers & AMODS_PROTECTED) {
        write_buffer(buffer, "protected");
        accessModifiers &= ~AMODS_PROTECTED;
    }

    else if (accessModifiers & AMODS_PUBLIC) {
        write_buffer(buffer, "public");
        accessModifiers &= ~AMODS_PUBLIC;
    }

    // while (accessModifiers) {
    //     if (added)
    //         write_buffer(buffer, " ");

    //     if (accessModifiers & AMODS_PRIVATE) {
    //         write_buffer(buffer, "private");
    //         accessModifiers &= ~AMODS_PRIVATE;
    //     }

    //     else if (accessModifiers & AMODS_PROTECTED) {
    //         write_buffer(buffer, "protected");
    //         accessModifiers &= ~AMODS_PROTECTED;
    //     }

    //     else if (accessModifiers & AMODS_PUBLIC) {
    //         write_buffer(buffer, "public");
    //         accessModifiers &= ~AMODS_PUBLIC;
    //     }
    // }
}

// EMODS_NONE = 0,
// EMODS_STATIC = 1 << 0,
// EMODS_ABSTRACT = 1 << 1,
// EMODS_FINAL = 1 << 2

void writeTElementModifiers(bufferADT buffer, TElementModifiers elementModifiers, TClassType type) {
    if (elementModifiers & EMODS_NONE) {
        return;
    }

    TElementModifiers added = 0;

    while (elementModifiers) {
        if (added)
            write_buffer(buffer, " ");

        else if (elementModifiers & EMODS_ABSTRACT) {
            if (type == CTYPE_ABSTRACTCLASS) {
                write_buffer(buffer, "abstract");
            } else if (type == CTYPE_INTERFACE) {
                // nada
            }
            added = EMODS_ABSTRACT;
        }

        if (elementModifiers & EMODS_STATIC) {
            write_buffer(buffer, "static");
            added = EMODS_STATIC;
        }

        else if (elementModifiers & EMODS_FINAL) {
            write_buffer(buffer, "final");
            added = EMODS_FINAL;
        }
        elementModifiers &= ~added;
    }
}

void writeTParameterList(bufferADT buffer, TParameterList* paramList) {
    while (paramList != NULL) {
        writeTTypeName(buffer, paramList->typeName);
        write_buffer(buffer, " ");
        write_buffer(buffer, paramList->symbolName);
        paramList = paramList->next;
        if (paramList != NULL)
            write_buffer(buffer, ", ");
    }
}

void writeTClassElement(bufferADT buffer, TClassElement* element, TClassType type) {
    TMethodParameterList* methodParamList = element->parameterList;
    if (type = CTYPE_INTERFACE && methodParamList != NULL) {
        write_buffer(buffer, "default ");
    }
    if (element->accessModifiers != NULL) {
        writeTAccessModifiers(buffer, element->accessModifiers);
    write_buffer(buffer, " ");
    }
 
    
    writeTElementModifiers(buffer, element->elementModifiers, type);
    write_buffer(buffer, " ");
    writeTTypeName(buffer, element->typeName);
    write_buffer(buffer, " ");
    write_buffer(buffer, element->symbolName);
    
    if (methodParamList != NULL) {
        write_buffer(buffer, "(");
        writeTParameterList(buffer, methodParamList->parameterList);
        write_buffer(buffer, ")");
        if (element->inlineCode != NULL) {
            write_buffer(buffer, " {\n");
            writeTInlineContent(buffer, element->inlineCode);
            write_buffer(buffer, "}\n");
        } else {
            write_buffer(buffer, ";\n");
        }
        return;
    }
    TInlineContent* inlineContent = element->inlineCode;
    if (inlineContent != NULL) {
        write_buffer(buffer, " = ");
        writeTInlineContent(buffer, element->inlineCode);
    }
    write_buffer(buffer, ";\n");
}

void writeTClassBody(bufferADT buffer, TClassBody* body, TClassType type) {
    while (body != NULL) {
        if (body->comment != NULL) {
            write_buffer(buffer, "/*");
            writeTInlineContent(buffer, body->comment);
            write_buffer(buffer, "*/\n");
        } else if (body->element != NULL) {
            writeTClassElement(buffer, body->element, type);
        } else {
            abort();
        }
        body = body->next;
    }
}

void generateClassFile(TClassType type,
                       TTypeName* name,
                       TTypeName* extends,
                       TCommaSeparatedTypenames* implements,
                       TInlineImportList* imports,
                       TClassBody* body) {
    LogDebug("\tgenerateClassFile(%lu)", name);
    bufferADT buffer = init_buffer(name->symbolName);

    writeTInlineImportList(buffer, imports);

    switch (type) {
        case CTYPE_CLASS:
            write_buffer(buffer, "class ");
            break;
        case CTYPE_ABSTRACTCLASS:
            write_buffer(buffer, "abstract class ");
            break;
        case CTYPE_INTERFACE:
            write_buffer(buffer, "interface ");
            break;
    }

    writeTTypeName(buffer, name);
    if (extends != NULL) {
        write_buffer(buffer, " extends ");
        writeTTypeName(buffer, extends);
    }
    if (implements != NULL) {
        write_buffer(buffer, " implements ");
        writeTCommaSeparatedTypenames(buffer, implements);
    }
    write_buffer(buffer, " {\n");
    writeTClassBody(buffer, body, type);
    write_buffer(buffer, "}\n");
    generate_file(buffer);
}

void generateClassFiles(TUmlBody* body) {
    LogDebug("\tgenerateClassFiles(%lu)", body);
    while (body != NULL) {
        TClassDefinition* class = body->bodyContent;
        generateClassFile(class->type,
                          class->name,
                          class->extends,
                          class->implements,
                          class->imports,
                          class->body);
        body = body->next;
    }
}

void generateUmlFiles(TUml* uml) {
    LogDebug("\tgenerateUmlFiles(%lu)", uml);
    while (uml != NULL) {
        generateClassFiles(uml->body);
        uml = uml->next;
    }
}

/**
 * Esta acción se corresponde con el no-terminal que representa el símbolo
 * inicial de la gramática, y por ende, es el último en ser ejecutado, lo que
 * indica que efectivamente el programa de entrada se pudo generar con esta
 * gramática, o lo que es lo mismo, que el programa pertenece al lenguaje.
 */
int StartGrammarAction(TUml* uml) {
    LogDebug("\tStartGrammarAction(%lu)", uml);

    generateUmlFiles(uml);

    /*
     * "state" es una variable global que almacena el estado del compilador,
     * cuyo campo "succeed" indica si la compilación fue o no exitosa, la cual
     * es utilizada en la función "main".
     */
    state.succeed = true;
    /*
     * Por otro lado, "result" contiene el resultado de aplicar el análisis
     * sintáctico mediante Bison, y almacenar el nood raíz del AST construido
     * en esta variable. Para el ejemplo de la calculadora, no hay AST porque
     * la expresión se computa on-the-fly, y es la razón por la cual esta
     * variable es un simple entero, en lugar de un nodo.
     */
    state.result = 420;
    state.program = uml;
    return state.result;
}

TUml* UmlGrammarAction(const TUmlBody* body, const TUml* next) {
    LogDebug("\tUmlGrammarAction(%lu, %lu)", body, next);
    TUml* node = malloc(sizeof(TUml));
    node->body = body;
    node->next = next;
    return node;
}

const TUmlBody* UmlBodyGrammarAction(const TClassDefinition* classDefinition, const TUmlBody* next) {
    LogDebug("\tUmlBodyGrammarAction(%lu, %lu)", classDefinition, next);
    TUmlBody* node = malloc(sizeof(TUmlBody));
    node->bodyContent = classDefinition;
    node->next = next;
    return node;
}

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

const TClassDefinition* ClassDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TCommaSeparatedTypenames* implements, const TInlineImportList* imports, const TClassBody* body) {
    LogDebug("\tClassDefinitionGrammarAction(%lu, %lu, %lu, %lu, %lu)", name, extends, implements, imports, body);
    TClassDefinition* node = malloc(sizeof(TClassDefinition));
    node->name = name;
    node->type = CTYPE_CLASS;
    node->extends = extends;
    node->implements = implements;
    node->imports = imports;
    node->body = body;
    return node;
}

const TClassDefinition* InterfaceDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TInlineImportList* imports, const TClassBody* body) {
    LogDebug("\tInterfaceDefinitionGrammarAction(%s, %lu, %lu, %lu)", name, extends, imports, body);
    TClassDefinition* node = malloc(sizeof(TClassDefinition));
    node->name = name;
    node->type = CTYPE_INTERFACE;
    node->extends = extends;
    node->implements = NULL;
    node->imports = imports;
    node->body = body;
    return node;
}

const TTypeName* ExtendsGrammarAction(const TTypeName* type) {
    LogDebug("\tExtendsGrammarAction(%lu)", type);
    return type;
}

const TCommaSeparatedTypenames* ImplementsGrammarAction(const TCommaSeparatedTypenames* commaSeparatedTypenames) {
    LogDebug("\tImplementsGrammarAction(%lu)", commaSeparatedTypenames);
    return commaSeparatedTypenames;
}

const TClassBody* ClassBodyGrammarAction(TClassBody* body, const TClassBody* next) {
    LogDebug("\tClassBodyGrammarAction(%lu, %lu)", body, next);
    body->next = next;
    return body;
}

TClassBody* ClassBodyContentGrammarAction(const TAccessModifiers accessMods, TClassElement* element) {
    LogDebug("\tClassBodyContentGrammarAction(%d, %lu)", accessMods, element);
    element->accessModifiers = accessMods;
    TClassBody* node = malloc(sizeof(TClassBody));
    node->comment = NULL;
    node->element = element;
    node->next = NULL;
    return node;
}

TClassBody* ClassInlineCommentGrammarAction(const TInlineContent* content) {
    LogDebug("\tClassInlineCommentGrammarAction(%lu)", content);
    TClassBody* node = malloc(sizeof(TClassBody));
    node->comment = content;
    LogDebug("\t%s\n", content->content);
    node->element = NULL;
    node->next = NULL;
    return node;
}

TClassElement* ClassConstructorGrammarAction(const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("\tClassConstructorGrammarAction(%d, %d, %d)", name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = AMODS_NONE;
    node->elementModifiers = EMODS_NONE;
    node->typeName = NULL;
    node->symbolName = name;
    node->parameterList = params;
    node->inlineCode = inlineCode;
    return node;
}

TClassElement* ClassElementGrammarAction(const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("\tClassElementGrammarAction(%d, %lu, %lu, %lu, %lu)", elementMods, type, name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = AMODS_NONE;
    node->elementModifiers = elementMods;
    node->typeName = type;
    node->symbolName = name;
    node->parameterList = params;
    node->inlineCode = inlineCode;
    return node;
}

const TClassBody* InterfaceBodyGrammarAction(TClassBody* body, const TClassBody* next) {
    LogDebug("\tInterfaceBodyGrammarAction(%lu, %lu)", body, next);
    body->next = next;
    return body;
}

TClassBody* InterfaceBodyContentGrammarAction(const TAccessModifiers accessMods, const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("\tInterfaceBodyContentGrammarAction(%d, %d, %lu, %lu, %lu, %lu)", accessMods, elementMods, type, name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = accessMods;
    node->elementModifiers = elementMods;
    node->typeName = type;
    node->symbolName = name;
    node->parameterList = params;
    node->inlineCode = inlineCode;

    TClassBody* body = malloc(sizeof(TClassBody));
    body->comment = NULL;
    body->element = node;
    body->next = NULL;

    return body;
}

/* -V-------------------------------------- Methods --------------------------------------V- */

const TMethodParameterList* MethodParamsGrammarAction(const TParameterList* paramList) {
    LogDebug("\tMethodParamsGrammarAction(%lu)", paramList);
    TMethodParameterList* node = malloc(sizeof(TMethodParameterList));
    node->parameterList = paramList;
    return node;
}

const TParameterList* ParameterGrammarAction(const TTypeName* type, const char* name) {
    LogDebug("\tParameterGrammarAction(%lu, %lu)", type, name);
    TParameterList* node = malloc(sizeof(TParameterList));
    node->typeName = type;
    node->symbolName = name;
    node->next = NULL;
    return node;
}

const TParameterList* ParameterListGrammarAction(const TTypeName* type, const char* name, const TParameterList* next) {
    LogDebug("\tParameterListGrammarAction(%lu, %lu, %lu)", type, name, next);
    TParameterList* node = malloc(sizeof(TParameterList));
    node->typeName = type;
    node->symbolName = name;
    node->next = next;
    return node;
}

/* -V-------------------------------------- Modifiers --------------------------------------V- */

TAccessModifiers DefaultGrammarAction() {
    LogDebug("\tDefaultGrammarAction()");
    return AMODS_DEFAULT;
}

TAccessModifiers PrivateGrammarAction() {
    LogDebug("\tPrivateGrammarAction()");
    return AMODS_PRIVATE;
}

TAccessModifiers ProtectedGrammarAction() {
    LogDebug("\tProtectedGrammarAction()");
    return AMODS_PROTECTED;
}

TAccessModifiers PublicGrammarAction() {
    LogDebug("\tPublicGrammarAction()");
    return AMODS_PUBLIC;
}

TElementModifiers AbstractGrammarAction() {
    LogDebug("\tAbstractGrammarAction()");
    return EMODS_ABSTRACT;
}

TElementModifiers StaticGrammarAction() {
    LogDebug("\tStaticGrammarAction()");
    return EMODS_STATIC;
}

TElementModifiers FinalGrammarAction() {
    LogDebug("\tFinalGrammarAction()");
    return EMODS_FINAL;
}

/* -V-------------------------------------- Misc --------------------------------------V- */

const char* SymbolnameGrammarAction(const char* symbol) {
    LogDebug("\tSymbolnameGrammarAction(%s)", symbol);
    return symbol;
}

const TTypeName* TypenameGrammarAction(const char* name) {
    LogDebug("\tTypenameGrammarAction(%s)", name);
    TTypeName* node = malloc(sizeof(TTypeName));
    node->symbolName = name;
    node->genericType = NULL;
    return node;
}

const TTypeName* GenericTypenameGrammarAction(const char* name, const TCommaSeparatedTypenames* genericType) {
    LogDebug("\tGenericTypenameGrammarAction(%s, %lu)", name, genericType);
    TTypeName* node = malloc(sizeof(TTypeName));
    node->symbolName = name;
    node->genericType = genericType;
    return node;
}

const TCommaSeparatedTypenames* CommaSeparatedTypenameGrammarAction(const TTypeName* type) {
    LogDebug("\tCommaSeparatedTypenameGrammarAction(%lu)", type);
    TCommaSeparatedTypenames* node = malloc(sizeof(TCommaSeparatedTypenames));
    node->typeName = type;
    node->next = NULL;
    return node;
}

const TCommaSeparatedTypenames* CommaSeparatedTypenamesGrammarAction(const TTypeName* type, const TCommaSeparatedTypenames* next) {
    LogDebug("\tCommaSeparatedTypenamesGrammarAction(%lu, %lu)", type, next);
    TCommaSeparatedTypenames* node = malloc(sizeof(TCommaSeparatedTypenames));
    node->typeName = type;
    node->next = next;
    return node;
}

/* -V-------------------------------------- Inlines --------------------------------------V- */

const TInlineContent* InlineContentGrammarAction(const char* content) {
    LogDebug("\tInlineContentGrammarAction(%s)", content);
    TInlineContent* node = malloc(sizeof(TInlineContent));
    node->content = content;
    node->next = NULL;
    return node;
}

const TInlineContent* InlineContentsGrammarAction(const char* content, const TInlineContent* next) {
    LogDebug("\tInlineContentsGrammarAction(%s, %lu)", content, next);
    TInlineContent* node = malloc(sizeof(TInlineContent));
    node->content = content;
    node->next = next;
    return node;
}

const TInlineContent* InlineCodeGrammarAction(const TInlineContent* content) {
    LogDebug("\tInlineCodeGrammarAction(%lu)", content);
    return content;
}

const TInlineContent* InlineCommentGrammarAction(const TInlineContent* content) {
    LogDebug("\tInlineCommentGrammarAction(%lu)", content);
    return content;
}

const TInlineContent* InlineImportGrammarAction(const TInlineContent* content) {
    LogDebug("\tInlineImportGrammarAction(%lu)", content);
    return content;
}

const TInlineImportList* InlineImportListGrammarAction(const TInlineContent* content, const TInlineImportList* next) {
    LogDebug("\tInlineImportListGrammarAction(%lu, %lu)", content, next);
    TInlineImportList* node = malloc(sizeof(TInlineImportList));
    node->import = content;
    node->next = next;
    return node;
}
