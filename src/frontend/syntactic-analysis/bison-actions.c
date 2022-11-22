#include "../../backend/support/logger.h"
#include "bison-actions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../utils/buffer.h"
#include "../../utils/symbol_table.h"

static TUml* root;

static void free_TTypeName(TTypeName* typeName);

static void free_TCommaSeparatedTypenames(TCommaSeparatedTypenames* typeNames) {
    LogDebug("free_TCommaSeparatedTypenames");
    if (typeNames == NULL)
        return;
    free_TCommaSeparatedTypenames(typeNames->next);
    free_TTypeName(typeNames->typeName);
    free(typeNames);
}

static void free_TTypeName(TTypeName* typeName) {
    if (typeName == NULL) {
        return;
    }
    LogDebug("free_TTypeName");
    free_TCommaSeparatedTypenames(typeName->genericType);
    free(typeName->symbolName);
    free(typeName);
}

static void free_TInlineContent(TInlineContent* inlineContent) {
    LogDebug("free_TInlineContent");
    if (inlineContent == NULL)
        return;
    free_TInlineContent(inlineContent->next);
    free(inlineContent);
}

static void free_TInlineImportList(TInlineImportList* list) {
    LogDebug("free_TInlineImportList");
    if (list == NULL) {
        return;
    }
    free_TInlineImportList(list->next);
    free_TInlineContent(list->import);
    free(list);
}

static void free_TParamList(TParameterList* paramList) {
    LogDebug("free_TParamList");
    if (paramList == NULL) {
        return;
    }

    free_TParamList(paramList->next);
    free_TTypeName(paramList->typeName);
    free(paramList->symbolName);
    free(paramList);
}

static void free_TMethodParameterList(TMethodParameterList* paramList) {
    LogDebug("free_TMethodParameterList");
    if (paramList != NULL) {
        free_TParamList(paramList->parameterList);
    }
}

static void free_TClassElement(TClassElement* element) {
    LogDebug("free_TClassElement");
    if (element == NULL)
        return;

    free_TInlineContent(element->inlineCode);
    free_TMethodParameterList(element->parameterList);
    free(element->symbolName);
    free_TTypeName(element->typeName);
}

static void free_TClassBody(TClassBody* body) {
    LogDebug("free_TClassBody");
    if (body == NULL)
        return;
    free_TClassBody(body->next);
    free_TInlineContent(body->comment);
    free_TClassElement(body->element);
    free(body);
}

static void free_TClassDefinition(TClassDefinition* class) {
    LogDebug("free_TClassDefinition");
    free_TTypeName(class->extends);
    free_TCommaSeparatedTypenames(class->implements);
    free_TInlineImportList(class->imports);
    free_TTypeName(class->name);
    free_TClassBody(class->body);
}

static void free_TUmlBody(TUmlBody* body) {
    LogDebug("free_TUmlBody");
    if (body == NULL) {
        return;
    }
    free_TUmlBody(body->next);
    free_TClassDefinition(body->bodyContent);
    free(body);
}

static void clean_resources() {
    TUml* root = (TUml*)state.program;
    LogDebug("clean_resources");
    while (root != NULL) {
        free_TUmlBody(root->body);
        TUml* aux = root;
        root = root->next;
        free(aux);
    }
}

static void clean_resources_and_exit() {
    LogInfo("exiting");
    clean_resources();
    state.result = -1;
    exit(-1);
}

/**
 * Implementación de "bison-grammar.h".
 */

/**
 * Esta función se ejecuta cada vez que se emite un error de sintaxis.
 */
void yyerror(const char* string) {
    LogError("Mensaje: '%s' debido a '%s' (linea %d).", string, yytext, yylineno);
    LogError("En ASCII es:");
    LogErrorRaw("");
    const int length = strlen(yytext);
    for (int i = 0; i < length; ++i) {
        LogErrorRaw("[%d]", yytext[i]);
    }
    LogErrorRaw("\n\n");
}

void writeGenerics(const bufferADT buffer, const TCommaSeparatedTypenames* generics) {
    LogDebug("writeGenerics(%lu)", generics);
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

void writeTTypeName(const bufferADT buffer, const TTypeName* name) {
    LogDebug("writeTTypeName(%lu)", name);
    if (name == NULL)
        return;

    write_buffer(buffer, name->symbolName);
    writeGenerics(buffer, name->genericType);
}

void writeTCommaSeparatedTypenames(const bufferADT buffer, const TCommaSeparatedTypenames* names) {
    LogDebug("writeTCommaSeparatedTypenames(%lu)", names);
    while (names != NULL) {
        writeTTypeName(buffer, names->typeName);
        names = names->next;
        if (names != NULL)
            write_buffer(buffer, ", ");
    }
}

void writeTInlineContent(const bufferADT buffer, const TInlineContent* content) {
    LogDebug("writeTInlineContent(%lu)", content);
    while (content != NULL) {
        LogDebug("content(%lu)", content->content);
        write_buffer(buffer, content->content);
        content = content->next;
    }
}

void writeTInlineImportList(const bufferADT buffer, const TInlineImportList* imports) {
    LogDebug("writeTInlineImportList(%lu)", imports);
    while (imports != NULL) {
        if (add_entry(imports->import->content, TYPE_IMPORT)) {
            LogError("entry for import %s is repeated", imports->import->content);
            state.result = -1;
            return;
        }
        write_buffer(buffer, "import ");
        writeTInlineContent(buffer, imports->import);
        write_buffer(buffer, ";\n");
        imports = imports->next;
    }
}

void writeTAccessModifiers(const bufferADT buffer, const TAccessModifiers accessModifiers) {
    LogDebug("writeTAccessModifiers(%lu, %lu)", buffer, accessModifiers);
    if (accessModifiers & AMODS_NONE || accessModifiers & AMODS_DEFAULT) {
        return;
    }

    TAccessModifiers auxModifiers = accessModifiers;

    // uint8_t added = 0;
    // TAccessModifiers added = 0;

    if (accessModifiers & AMODS_PRIVATE) {
        write_buffer(buffer, "private");
        auxModifiers &= ~AMODS_PRIVATE;
    }

    else if (accessModifiers & AMODS_PROTECTED) {
        write_buffer(buffer, "protected");
        auxModifiers &= ~AMODS_PROTECTED;
    }

    else if (accessModifiers & AMODS_PUBLIC) {
        write_buffer(buffer, "public");
        auxModifiers &= ~AMODS_PUBLIC;
    }
}

void writeTElementModifiers(const bufferADT buffer, const TElementModifiers elementModifiers, const TClassType type) {
    int a = elementModifiers;
    LogDebug("writeTElementModifiers(%lu, %d)", buffer, a);
    if (elementModifiers & EMODS_NONE) {
        return;
    }

    TElementModifiers added = 0;
    TElementModifiers auxElementModifiers = elementModifiers;

    while (auxElementModifiers) {
        // LogDebug("%d", elementModifiers);
        if (added)
            write_buffer(buffer, " ");

        else if (auxElementModifiers & EMODS_ABSTRACT) {
            if (type == CTYPE_ABSTRACTCLASS) {
                write_buffer(buffer, "abstract");
            } else if (type == CTYPE_INTERFACE) {
                // nada
            }
            added = EMODS_ABSTRACT;
        }

        if (auxElementModifiers & EMODS_STATIC) {
            if (type != CTYPE_INTERFACE) {
                write_buffer(buffer, "static");
            }
            added = EMODS_STATIC;
        }

        else if (auxElementModifiers & EMODS_FINAL) {
            write_buffer(buffer, "final");
            added = EMODS_FINAL;
        }
        auxElementModifiers &= ~added;
    }
}

void writeTParameterList(const bufferADT buffer, const TParameterList* paramList) {
    LogDebug("writeTParameterList(%lu, %lu)", buffer, paramList);
    while (paramList != NULL) {
        writeTTypeName(buffer, paramList->typeName);
        write_buffer(buffer, " ");
        write_buffer(buffer, paramList->symbolName);
        paramList = paramList->next;
        if (paramList != NULL)
            write_buffer(buffer, ", ");
    }
}

void writeDefaultReturn(const bufferADT buffer, const char* type) {
    if (strcmp(type, "void") == 0) {
        write_buffer(buffer, "return;");
    } else if (strcmp(type, "int") == 0 || strcmp(type, "short") == 0 || strcmp(type, "long") == 0 || strcmp(type, "char") == 0) {
        write_buffer(buffer, "return 0;");
    } else if (strcmp(type, "float") == 0 || strcmp(type, "double") == 0) {
        write_buffer(buffer, "return 0.0;");
    } else if (strcmp(type, "boolean") == 0) {
        write_buffer(buffer, "return false;");
    } else {
        write_buffer(buffer, "return null;");
    }
}

void writeTClassElement(const bufferADT buffer, const TClassElement* element, const TClassType type) {
    LogDebug("writeTClassElement(%lu, %lu)", buffer, element);
    TMethodParameterList* methodParamList = element->parameterList;
    TClassType auxType = type;
    if (auxType == CTYPE_INTERFACE && !(element->elementModifiers & EMODS_ABSTRACT)) {
        if (!(element->elementModifiers & EMODS_STATIC)) {
            write_buffer(buffer, "default ");
        }
    }

    if (element->accessModifiers != AMODS_NONE) {
        writeTAccessModifiers(buffer, element->accessModifiers);
        write_buffer(buffer, " ");
    }

    writeTElementModifiers(buffer, element->elementModifiers, type);
    write_buffer(buffer, " ");
    writeTTypeName(buffer, element->typeName);
    write_buffer(buffer, " ");
    write_buffer(buffer, element->symbolName);

    if (methodParamList != NULL) {
        if (add_entry_method(element->symbolName, methodParamList)) {
            LogError("entry for method %s is repeated", element->symbolName);
            state.result = -1;
            state.succeed = false;
        }

        write_buffer(buffer, "(");
        writeTParameterList(buffer, methodParamList->parameterList);
        write_buffer(buffer, ")");

        if (!(element->elementModifiers & EMODS_ABSTRACT)) {
            write_buffer(buffer, " {\n");
            if (element->inlineCode != NULL) {
                writeTInlineContent(buffer, element->inlineCode);
            } else if (!is_method_constructor(element->symbolName)) {
                writeDefaultReturn(buffer, element->symbolName);
            }

            write_buffer(buffer, "}\n");
        } else {
            write_buffer(buffer, ";\n");
        }
        return;
    }
    if (add_entry(element->symbolName, TYPE_CLASS_ELEM)) {
        LogError("entry for variable %s is repeated", element->symbolName);
        state.result = -1;
        state.succeed = false;
    }
    TInlineContent* inlineContent = element->inlineCode;
    if (inlineContent != NULL) {
        write_buffer(buffer, " = ");
        writeTInlineContent(buffer, element->inlineCode);
        write_buffer(buffer, "\n");
        return;
    }
    write_buffer(buffer, ";\n");
}

void writeTClassBody(const bufferADT buffer, const TClassBody* body, const TClassType type) {
    LogDebug("writeTClassBody(%lu, %lu)", buffer, body);
    while (body != NULL) {
        if (body->comment != NULL) {
            write_buffer(buffer, "/*");
            writeTInlineContent(buffer, body->comment);
            write_buffer(buffer, "*/\n");
        } else if (body->element != NULL) {
            writeTClassElement(buffer, body->element, type);
        } else {
            LogErrorRaw("Unexpected class element type");
            state.result = -1;
            state.succeed = false;
        }
        body = body->next;
    }
}

// void checkValidImplements(const TCommaSeparatedTypenames* implements) {
//     TCommaSeparatedTypenames * auxImplements = (TCommaSeparatedTypenames*) implements;
//     while (auxImplements != NULL) {
//         if (!is_valid_implements_class(implements->))
//     }
// }

void generateClassFile(const TClassType type,
                       const TTypeName* name,
                       const TTypeName* extends,
                       const TCommaSeparatedTypenames* implements,
                       const TInlineImportList* imports,
                       const TClassBody* body) {

    LogDebug("generateClassFile(%lu)", name);
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
        if (!is_valid_extends(extends, type)) {
            LogError("Not a valid entity to extend");
            state.result = -1;
            state.succeed = false;
        }
        writeTTypeName(buffer, extends);
    }
    if (implements != NULL) {
        write_buffer(buffer, " implements ");
        if (!is_valid_implements(implements)) {
            LogError("Not a valid entity to implement");
            state.result = -1;
            state.succeed = false;
        }
        writeTCommaSeparatedTypenames(buffer, implements);
    }
    write_buffer(buffer, " {\n");
    writeTClassBody(buffer, body, type);
    write_buffer(buffer, "}\n");
    if (!state.result) {
        generate_file(buffer);
    }
    destroy_buffer(buffer);
}

void generateClassFiles(const TUmlBody* body) {
    LogDebug("generateClassFiles(%lu)", body);
    while (body != NULL) {
        new_class();
        TClassDefinition* class = body->bodyContent;
        TSymbolType type = TYPE_CLASS | (class->type == CTYPE_INTERFACE ? TYPE_INTERFACE : 0);

        if (add_entry(class->name->symbolName, type)) {
            LogError("%s name is already used for class type", class->name->symbolName);
            state.result = -1;
            state.succeed = false;
        }
        if (state.result) {
            break;
        }
        generateClassFile(class->type,
                          class->name,
                          class->extends,
                          class->implements,
                          class->imports,
                          class->body);
        end_class();

        body = body->next;
    }
}

void generateUmlFiles(const TUml* uml) {
    LogDebug("generateUmlFiles(%lu)", uml);
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
int StartGrammarAction(const TUml* uml) {
    LogDebug("StartGrammarAction(%lu)", uml);
    state.program = uml;
    state.result = 0;
    state.succeed = true;
    init_symbol_table();
    generateUmlFiles(uml);
    destroy_symbol_table();
    clean_resources();
    /*
     * "state" es una variable global que almacena el estado del compilador,
     * cuyo campo "succeed" indica si la compilación fue o no exitosa, la cual
     * es utilizada en la función "main".
     */

    /*
     * Por otro lado, "result" contiene el resultado de aplicar el análisis
     * sintáctico mediante Bison, y almacenar el nood raíz del AST construido
     * en esta variable. Para el ejemplo de la calculadora, no hay AST porque
     * la expresión se computa on-the-fly, y es la razón por la cual esta
     * variable es un simple entero, en lugar de un nodo.
     */

    return state.result;
}

TUml* UmlGrammarAction(const TUmlBody* body, const TUml* next) {
    LogDebug("UmlGrammarAction(%lu, %lu)", body, next);
    TUml* node = malloc(sizeof(TUml));
    node->body = (TUmlBody*)body;
    node->next = (TUml*)next;
    return node;
}

const TUmlBody* UmlBodyGrammarAction(const TClassDefinition* classDefinition, const TUmlBody* next) {
    LogDebug("UmlBodyGrammarAction(%lu, %lu)", classDefinition, next);
    TUmlBody* node = malloc(sizeof(TUmlBody));
    node->bodyContent = (TClassDefinition*)classDefinition;
    node->next = (TUmlBody*)next;
    return node;
}

/* -V-------------------------------------- Classes & Interfaces --------------------------------------V- */

const TClassDefinition* ClassDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TCommaSeparatedTypenames* implements, const TInlineImportList* imports, const TClassBody* body) {
    LogDebug("ClassDefinitionGrammarAction(%lu, %lu, %lu, %lu, %lu)", name, extends, implements, imports, body);
    TClassDefinition* node = malloc(sizeof(TClassDefinition));
    node->name = (TTypeName*)name;
    node->type = CTYPE_CLASS;
    node->extends = (TTypeName*)extends;
    node->implements = (TCommaSeparatedTypenames*)implements;
    node->imports = (TInlineImportList*)imports;
    node->body = (TClassBody*)body;
    return node;
}

const TClassDefinition* AbstractClassDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TCommaSeparatedTypenames* implements, const TInlineImportList* imports, const TClassBody* body) {
    LogDebug("AbstractClassDefinitionGrammarAction(%lu, %lu, %lu, %lu, %lu)", name, extends, implements, imports, body);
    TClassDefinition* node = malloc(sizeof(TClassDefinition));
    node->name = (TTypeName*)name;
    node->type = CTYPE_ABSTRACTCLASS;
    node->extends = (TTypeName*)extends;
    node->implements = (TCommaSeparatedTypenames*)implements;
    node->imports = (TInlineImportList*)imports;
    node->body = (TClassBody*)body;
    return node;
}

const TClassDefinition* InterfaceDefinitionGrammarAction(const TTypeName* name, const TTypeName* extends, const TInlineImportList* imports, const TClassBody* body) {
    LogDebug("InterfaceDefinitionGrammarAction(%s, %lu, %lu, %lu)", name, extends, imports, body);
    TClassDefinition* node = malloc(sizeof(TClassDefinition));
    node->name = (TTypeName*)name;
    node->type = CTYPE_INTERFACE;
    node->extends = (TTypeName*)extends;
    node->implements = NULL;
    node->imports = (TInlineImportList*)imports;
    node->body = (TClassBody*)body;
    return node;
}

const TTypeName* ExtendsGrammarAction(const TTypeName* type) {
    LogDebug("ExtendsGrammarAction(%lu)", type);
    return type;
}

const TCommaSeparatedTypenames* ImplementsGrammarAction(const TCommaSeparatedTypenames* commaSeparatedTypenames) {
    LogDebug("ImplementsGrammarAction(%lu)", commaSeparatedTypenames);
    return commaSeparatedTypenames;
}

const TClassBody* ClassBodyGrammarAction(TClassBody* body, const TClassBody* next) {
    LogDebug("ClassBodyGrammarAction(%lu, %lu)", body, next);
    body->next = (TClassBody*)next;
    return body;
}

TClassBody* ClassBodyContentGrammarAction(const TAccessModifiers accessMods, TClassElement* element) {
    LogDebug("ClassBodyContentGrammarAction(%d, %lu)", accessMods, element);
    element->accessModifiers = (TAccessModifiers)accessMods;
    TClassBody* node = malloc(sizeof(TClassBody));
    node->comment = NULL;
    node->element = (TClassElement*)element;
    node->next = NULL;
    return node;
}

TClassBody* ClassInlineCommentGrammarAction(const TInlineContent* content) {
    LogDebug("ClassInlineCommentGrammarAction(%lu)", content);
    TClassBody* node = malloc(sizeof(TClassBody));
    node->comment = (TInlineContent*)content;
    LogDebug("%s\n", content->content);
    node->element = NULL;
    node->next = NULL;
    return node;
}

TClassElement* ClassConstructorGrammarAction(const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("ClassConstructorGrammarAction(%d, %d, %d)", name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = AMODS_NONE;
    node->elementModifiers = EMODS_NONE;
    node->typeName = NULL;
    node->symbolName = (char*)name;
    node->parameterList = (TMethodParameterList*)params;
    node->inlineCode = (TInlineContent*)inlineCode;
    return node;
}

TClassElement* ClassElementGrammarAction(const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("ClassElementGrammarAction(%u, %lu, %lu, %lu, %lu)", elementMods, type, name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = AMODS_NONE;
    node->elementModifiers = (TElementModifiers)elementMods;
    node->typeName = (TTypeName*)type;
    node->symbolName = (char*)name;
    node->parameterList = (TMethodParameterList*)params;
    node->inlineCode = (TInlineContent*)inlineCode;
    return node;
}

const TClassBody* InterfaceBodyGrammarAction(TClassBody* body, const TClassBody* next) {
    LogDebug("InterfaceBodyGrammarAction(%lu, %lu)", body, next);
    body->next = (TClassBody*)next;
    return body;
}

TClassBody* InterfaceBodyContentGrammarAction(const TAccessModifiers accessMods, const TElementModifiers elementMods, const TTypeName* type, const char* name, const TMethodParameterList* params, const TInlineContent* inlineCode) {
    LogDebug("InterfaceBodyContentGrammarAction(%d, %d, %lu, %lu, %lu, %lu)", accessMods, elementMods, type, name, params, inlineCode);
    TClassElement* node = malloc(sizeof(TClassElement));
    node->accessModifiers = (TAccessModifiers)accessMods;
    node->elementModifiers = (TElementModifiers)elementMods;
    node->typeName = (TTypeName*)type;
    node->symbolName = (char*)name;
    node->parameterList = (TMethodParameterList*)params;
    node->inlineCode = (TInlineContent*)inlineCode;

    TClassBody* body = malloc(sizeof(TClassBody));
    body->comment = NULL;
    body->element = node;
    body->next = NULL;

    return body;
}

/* -V-------------------------------------- Methods --------------------------------------V- */

const TMethodParameterList* MethodParamsGrammarAction(const TParameterList* paramList) {
    LogDebug("MethodParamsGrammarAction(%lu)", paramList);
    TMethodParameterList* node = malloc(sizeof(TMethodParameterList));
    node->parameterList = (TParameterList*)paramList;
    return node;
}

const TParameterList* ParameterGrammarAction(const TTypeName* type, const char* name) {
    LogDebug("ParameterGrammarAction(%lu, %lu)", type, name);
    TParameterList* node = malloc(sizeof(TParameterList));
    node->typeName = (TTypeName*)type;
    node->symbolName = (char*)name;
    node->next = NULL;
    return node;
}

const TParameterList* ParameterListGrammarAction(const TTypeName* type, const char* name, const TParameterList* next) {
    LogDebug("ParameterListGrammarAction(%lu, %lu, %lu)", type, name, next);
    TParameterList* node = malloc(sizeof(TParameterList));
    node->typeName = (TTypeName*)type;
    node->symbolName = (char*)name;
    node->next = (TParameterList*)next;
    return node;
}

/* -V-------------------------------------- Modifiers --------------------------------------V- */

TAccessModifiers DefaultGrammarAction() {
    LogDebug("DefaultGrammarAction()");
    return AMODS_DEFAULT;
}

TAccessModifiers PrivateGrammarAction() {
    LogDebug("PrivateGrammarAction()");
    return AMODS_PRIVATE;
}

TAccessModifiers ProtectedGrammarAction() {
    LogDebug("ProtectedGrammarAction()");
    return AMODS_PROTECTED;
}

TAccessModifiers PublicGrammarAction() {
    LogDebug("PublicGrammarAction()");
    return AMODS_PUBLIC;
}

TElementModifiers AbstractGrammarAction() {
    LogDebug("AbstractGrammarAction()");
    return EMODS_ABSTRACT;
}

TElementModifiers StaticGrammarAction() {
    LogDebug("StaticGrammarAction()");
    return EMODS_STATIC;
}

TElementModifiers FinalGrammarAction() {
    LogDebug("FinalGrammarAction(%d)", EMODS_FINAL);
    return EMODS_FINAL;
}

/* -V-------------------------------------- Misc --------------------------------------V- */

const char* SymbolnameGrammarAction(const char* symbol) {
    LogDebug("SymbolnameGrammarAction(%s)", symbol);
    return symbol;
}

const TTypeName* TypenameGrammarAction(const char* name) {
    LogDebug("TypenameGrammarAction(%s)", name);
    TTypeName* node = malloc(sizeof(TTypeName));
    node->symbolName = (char*)name;
    node->genericType = NULL;
    return node;
}

const TTypeName* GenericTypenameGrammarAction(const char* name, const TCommaSeparatedTypenames* genericType) {
    LogDebug("GenericTypenameGrammarAction(%s, %lu)", name, genericType);
    TTypeName* node = malloc(sizeof(TTypeName));
    node->symbolName = (char*)name;
    node->genericType = (TCommaSeparatedTypenames*)genericType;
    return node;
}

const TCommaSeparatedTypenames* CommaSeparatedTypenameGrammarAction(const TTypeName* type) {
    LogDebug("CommaSeparatedTypenameGrammarAction(%lu)", type);
    TCommaSeparatedTypenames* node = malloc(sizeof(TCommaSeparatedTypenames));
    node->typeName = (TTypeName*)type;
    node->next = NULL;
    return node;
}

const TCommaSeparatedTypenames* CommaSeparatedTypenamesGrammarAction(const TTypeName* type, const TCommaSeparatedTypenames* next) {
    LogDebug("CommaSeparatedTypenamesGrammarAction(%lu, %lu)", type, next);
    TCommaSeparatedTypenames* node = malloc(sizeof(TCommaSeparatedTypenames));
    node->typeName = (TTypeName*)type;
    node->next = (TCommaSeparatedTypenames*)next;
    return node;
}

/* -V-------------------------------------- Inlines --------------------------------------V- */

const TInlineContent* InlineContentGrammarAction(const char* content) {
    LogDebug("InlineContentGrammarAction(%s)", content);
    TInlineContent* node = malloc(sizeof(TInlineContent));
    node->content = (char*)content;
    node->next = NULL;
    return node;
}

const TInlineContent* InlineContentsGrammarAction(const char* content, const TInlineContent* next) {
    LogDebug("InlineContentsGrammarAction(%s, %lu)", content, next);
    TInlineContent* node = malloc(sizeof(TInlineContent));
    node->content = (char*)content;
    node->next = (TInlineContent*)next;
    return node;
}

const TInlineContent* InlineCodeGrammarAction(const TInlineContent* content) {
    LogDebug("InlineCodeGrammarAction(%lu)", content);
    return content;
}

const TInlineContent* InlineCommentGrammarAction(const TInlineContent* content) {
    LogDebug("InlineCommentGrammarAction(%lu)", content);
    return content;
}

const TInlineContent* InlineImportGrammarAction(const TInlineContent* content) {
    LogDebug("InlineImportGrammarAction(%lu)", content);
    return content;
}

const TInlineImportList* InlineImportListGrammarAction(const TInlineContent* content, const TInlineImportList* next) {
    LogDebug("InlineImportListGrammarAction(%lu, %lu)", content, next);
    TInlineImportList* node = malloc(sizeof(TInlineImportList));
    node->import = (TInlineContent*)content;
    node->next = (TInlineImportList*)next;
    return node;
}
