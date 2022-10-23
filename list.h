#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <malloc.h>

#ifndef _DEBUG
#define _DEBUG 0
#endif

typedef int Elem_t;

const Elem_t POISON = 0xBEEF;

#if _DEBUG
typedef void (*PrintFunction)(FILE *file, Elem_t value);
#endif

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
};

struct ListElement_t {
    Elem_t value    = POISON;
    long previous = POISON;
    long next     = POISON;
};

struct List_t {
    ListElement_t *values        = {};
    size_t        header         = POISON;
    size_t        tail           = POISON;
    size_t        free           = POISON;

    size_t        size           = POISON;
};

void ListCtor(List_t *list, size_t listSize, int *err = nullptr);

int ListVerify(List_t *list);

void ListDtor(List_t *list, int *err = nullptr);

#if _DEBUG
void closeLogfile(void);
#endif

#endif