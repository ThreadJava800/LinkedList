#include "list.h"

#if _DEBUG
FILE *logFile = fopen("logs.txt", "w");
int   onClose = atexit(closeLogfile);
#endif

void ListCtor(List_t *list, size_t listSize, int *err) {
    if (!list) {
        if (err) *err = LIST_NULL;
        return;
    }

    list->values = (ListElement_t*) calloc(listSize, sizeof(ListElement_t));
    if (!list->values) {
        if (err) *err = CANNOT_ALLOC_MEM;
        return;
    }
    list->size = listSize;

    list->values[0].value = POISON;
    list->values[0].previous = 0;
    list->values[0].next = 0;

    for (size_t i = 1; i < listSize; i++) {
        list->values[i].value    =  0;
        list->values[i].previous = -1;
        list->values[i].next     = -1;
    }

    list->free    = 1;
    list->header  = 0;
    list->tail    = 0;

    if (err) *err = ListVerify(list);
}

int ListVerify(List_t *list) {
    if (!list)                          return LIST_NULL;
    if (!list->values)                  return LIST_DATA_NULL;
    if (list->header == POISON)         return LIST_HEADER_POISONED;
    if (list->tail == POISON)           return LIST_TAIL_POISONED;
    if (list->free == POISON)           return LIST_FREE_POISONED;
    if (list->size == POISON)           return LIST_SIZE_POISONED;
    if (list->header > list->size)      return LIST_HEADER_POISONED;
    if (list->tail   > list->size)      return LIST_TAIL_POISONED;
    
    for (size_t i = 1; i < list->size; i++) {
        if (list->values[i].value == POISON)    return ELEM_VALUE_POISONED;
        if (list->values[i].next == POISON)     return ELEM_NEXT_POISONED;
        if (list->values[i].previous == POISON) return ELEM_PREV_POISONED;
    }

    return LIST_OK;
}

void ListDtor(List_t *list, int *err) {
    if (!list) {
        if (err) *err = LIST_NULL;
        return;
    }

    if (!list->values) {
        if (err) *err = LIST_DATA_NULL;
    } else {
        free(list->values);
    }

    list->header = POISON;
    list->tail   = POISON;
    list->free   = POISON;
    list->size   = POISON;
}

void closeLogfile(void) {
    fclose(logFile);
}