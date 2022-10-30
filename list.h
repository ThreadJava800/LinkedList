#ifndef LIST_H
#define LIST_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#ifndef _DEBUG
#define _DEBUG 0
#endif

typedef int Elem_t;

const Elem_t POISON = 0xBEEF;

const int RESIZE_COEFFICIENT = 2;

const int MAX_COMMAND_LENGTH = 2 << 8;

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
    LOSING_DATA           = -16,
    CANNOT_OPEN_FILE      = -17,
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
    ListElement_t *values      = {};
    long        free           = POISON;

    long        size           = POISON;
    long        capacity       = POISON;

    short         linearized   = 1;
    short         needLinear   = 1;

    #if _DEBUG
    ListDebug_t debugInfo = {};
    #endif
};

#define CHECK(expression, errCode) { \
    if (expression) {                 \
        DUMP(list, errCode);           \
        if (err) *err = errCode;        \
        exit(errCode);                   \
    }                                     \
}                                          \

void _listCtor(List_t *list, long listSize, short needLinear, int *err = nullptr);

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

void fillElemList(ListElement_t *listElems, long capacity, int *err = nullptr);

int listVerify(List_t *list);

long _listInsertPhys(List_t *list, Elem_t value, long index, int *err = nullptr);

long listInsert(List_t *list, Elem_t value, long index, int *err = nullptr);

long listPushBack(List_t *list, Elem_t value, int *err = nullptr);

long listPushFront(List_t *list, Elem_t value, int *err = nullptr);

Elem_t _listRemovePhys(List_t *list, long index, int *err = nullptr);

Elem_t listRemove(List_t *list, long index, int *err = nullptr);

[[nodiscard]] long logicToPhysics(List_t *list, long logicIndex, int *err = nullptr);

void listLinearize(List_t *list, int *err = nullptr);

void listResize(List_t *list, long newCapacity, int *err = nullptr);

int checkForPoisons(List_t *list, long newCapacity, int *err = nullptr);

void listRealloc(List_t *list, long newCapacity, int *err = nullptr);

void poisonList(List_t *list, long newCapacity, long oldCapacity, int *err = nullptr);

void listDtor(List_t *list, int *err = nullptr);

void visualGraph(List_t *list, const char *action = "");

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