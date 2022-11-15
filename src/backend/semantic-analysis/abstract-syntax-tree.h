#ifndef _ABSTRACT_SYNTAX_TREE_H_
#define _ABSTRACT_SYNTAX_TREE_H_

/* -V-------------------------------------- Inlines --------------------------------------V- */

typedef struct TInlineContent {
    const char* content;
    const struct TInlineContent* next;
} TInlineContent;

typedef struct TInlineImportList {
    const TInlineContent* import;
    const struct TInlineImportList* next;
} TInlineImportList;

/* -V-------------------------------------- General --------------------------------------V- */

struct TCommaSeparatedTypenames {
    const struct TTypeName* typeName;
    const struct TCommaSeparatedTypenames* next;
};

struct TTypeName {
    const char* symbolName;
    const struct TCommaSeparatedTypenames* genericType;
};

typedef struct TCommaSeparatedTypenames TCommaSeparatedTypenames;
typedef struct TTypeName TTypeName;

/* -V-------------------------------------- Class Elements --------------------------------------V- */

typedef enum {
    EMODS_NONE = 0,
    EMODS_STATIC = 1 << 0,
    EMODS_ABSTRACT = 1 << 1,
    EMODS_FINAL = 1 << 3
} TElementModifiers;

typedef enum {
    AMODS_NONE = 0,
    AMODS_DEFAULT = 1 << 0,
    AMODS_PRIVATE = 1 << 1,
    AMODS_PROTECTED = 1 << 2,
    AMODS_PUBLIC = 1 << 3
} TAccessModifiers;

typedef struct TParameterList {
    const TTypeName* typeName;
    const char* symbolName;
    const struct TParameterList* next;
} TParameterList;

typedef struct TMethodParameterList {
    const TParameterList* parameterList;
} TMethodParameterList;

/* -V-------------------------------------- Classes (& Interfaces) --------------------------------------V- */

typedef struct TClassElement {
    TAccessModifiers accessModifiers;
    TElementModifiers elementModifiers;
    const TTypeName* typeName;
    const char* symbolName;
    const TMethodParameterList* parameterList;
    const TInlineContent* inlineCode;
} TClassElement;

typedef struct TClassBody {
    const TClassElement* element;
    const TInlineContent* comment;
    const struct TClassBody* next;
} TClassBody;

typedef enum {
    CTYPE_CLASS = 0,
    CTYPE_ABSTRACTCLASS,
    CTYPE_INTERFACE
} TClassType;

typedef struct TClassDefinition {
    TClassType type;
    const TTypeName* name;
    const TTypeName* extends;
    const TCommaSeparatedTypenames* implements;
    const TInlineImportList* imports;
    const TClassBody* body;
} TClassDefinition;

/* -V-------------------------------------- UML --------------------------------------V- */

typedef struct TUmlBody {
    const TClassDefinition* bodyContent;
    const struct TUmlBody* next;
} TUmlBody;

typedef struct TUml {
    const TUmlBody* body;
    const struct TUml* next;
} TUml;

#endif
