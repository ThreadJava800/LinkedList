#ifndef LIST_H
#define LIST_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#ifndef _DEBUG
#define _DEBUG 0
#endif

typedef int Elem_t;

const Elem_t POISON = 0xBEEF;

enum ListErrors {
    LIST_OK               =  0,
    LIST_NULL             = -1,
    LIST_FULL             = -2,
    LIST_HEADER_POISONED  = -3,
    LIST_TAIL_POISONED    = -4,
    LIST_FREE_POISONED    = -5,
    LIST_SIZE_POISONED    = -6,
    LIST_DATA_NULL        = -7,
    ELEM_VALUE_POISONED   = -8,
    ELEM_PREV_POISONED    = -9,
    ELEM_NEXT_POISONED    = -10,

    CANNOT_ALLOC_MEM      = -11,
    INDEX_BIGGER_SIZE     = -12,
    INDEX_INCORRECT       = -13,
    NOTHING_TO_DELETE     = -14,
    ALREADY_POISON        = -15,
};

struct ListElement_t {
    Elem_t value  = POISON;
    long previous = POISON;
    long next     = POISON;
};

#if _DEBUG
struct ListDebug_t {
    const char *createFunc = nullptr; 
    const char *createFile = nullptr; 
    const char *name       = nullptr;
    int         createLine = 0;
};
#endif

struct List_t {
    ListElement_t *values        = {};
    size_t        header         = POISON;
    size_t        tail           = POISON;
    size_t        free           = POISON;

    size_t        size           = POISON;
    size_t        capacity       = POISON;

    short         linearized     = 1;

    #if _DEBUG
    ListDebug_t debugInfo = {};
    #endif
};

#define CHECK(expression, errCode) { \
    if (expression) {                 \
        DUMP(list, errCode);           \
        if (err) *err = errCode;        \
        abort();                         \
    }                                     \
}                                          \

void _listCtor(List_t *list, size_t listSize, int *err = nullptr);

#if _DEBUG

    #define listCtor(list, ...) {                               \
        if (list) {                                              \
            (list)->debugInfo.createFunc = __PRETTY_FUNCTION__;   \
            (list)->debugInfo.createFile = __FILE_NAME__;          \
            (list)->debugInfo.createLine = __LINE__;                \
            (list)->debugInfo.name       = #list;                    \
        }                                                             \
        _listCtor(list, ##__VA_ARGS__);                                \
    }                                                                   \

#else 

    #define listCtor(list, ...) {         \
        _listCtor(list, ##__VA_ARGS__);    \
    }                                       \

#endif

void fillElemList(ListElement_t *listElems, size_t capacity, int *err = nullptr);

int listVerify(List_t *list);

void _listInsertPhys(List_t *list, Elem_t value, size_t index, int *err = nullptr);

void listInsert(List_t *list, Elem_t value, size_t index, int *err = nullptr);

Elem_t _listRemovePhys(List_t *list, size_t index, int *err = nullptr);

Elem_t listRemove(List_t *list, size_t index, int *err = nullptr);

[[nodiscard]] size_t logicToPhysics(List_t *list, size_t logicIndex, int *err = nullptr);

void listLinearize(List_t *list, int *err = nullptr);

void listDtor(List_t *list, int *err = nullptr);

#if _DEBUG

    void mprintf(FILE *file, const char *fmt...);

    void dumpList(List_t *list, int errorCode, const char *fileName, const char *function, int line);

    #define DUMP(list, errorCode) {                                                 \
        dumpList(list, errorCode, __FILE_NAME__, __PRETTY_FUNCTION__, __LINE__);     \
    }                                                                                 \

    void closeLogfile(void);

#else 

    #define DUMP(list, errorCode) {}

#endif

#endif