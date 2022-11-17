#ifndef _ABSTRACT_SYNTAX_TREE_H_
#define _ABSTRACT_SYNTAX_TREE_H_

/* -V-------------------------------------- Inlines --------------------------------------V- */

typedef struct TInlineContent {
    char* content;
    struct TInlineContent* next;
} TInlineContent;

typedef struct TInlineImportList {
    TInlineContent* import;
    struct TInlineImportList* next;
} TInlineImportList;

/* -V-------------------------------------- General --------------------------------------V- */

struct TCommaSeparatedTypenames {
    struct TTypeName* typeName;
    struct TCommaSeparatedTypenames* next;
};

struct TTypeName {
    char* symbolName;
    struct TCommaSeparatedTypenames* genericType;
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
    TTypeName* typeName;
    char* symbolName;
    struct TParameterList* next;
} TParameterList;

typedef struct TMethodParameterList {
    TParameterList* parameterList;
} TMethodParameterList;

/* -V-------------------------------------- Classes (& Interfaces) --------------------------------------V- */

typedef struct TClassElement {
    TAccessModifiers accessModifiers;
    TElementModifiers elementModifiers;
     TTypeName* typeName;
     char* symbolName;
     TMethodParameterList* parameterList;
     TInlineContent* inlineCode;
} TClassElement;

typedef struct TClassBody {
    TClassElement* element;
     TInlineContent* comment;
     struct TClassBody* next;
} TClassBody;

typedef enum {
    CTYPE_CLASS = 0,
    CTYPE_ABSTRACTCLASS,
    CTYPE_INTERFACE
} TClassType;

typedef struct TClassDefinition {
    TClassType type;
     TTypeName* name;
     TTypeName* extends;
     TCommaSeparatedTypenames* implements;
     TInlineImportList* imports;
     TClassBody* body;
} TClassDefinition;

/* -V-------------------------------------- UML --------------------------------------V- */

typedef struct TUmlBody {
     TClassDefinition* bodyContent;
     struct TUmlBody* next;
} TUmlBody;

typedef struct TUml {
     TUmlBody* body;
     struct TUml* next;
} TUml;

#endif
