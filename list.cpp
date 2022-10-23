#include "list.h"

#if _DEBUG
FILE *logFile = fopen("logs.txt", "w");
int   onClose = atexit(closeLogfile);
#endif

void _listCtor(List_t *list, size_t listSize, int *err) {
    CHECK(!list, LIST_NULL);

    list->values = (ListElement_t*) calloc(listSize, sizeof(ListElement_t));
    CHECK(!list->values, CANNOT_ALLOC_MEM);
    list->size = listSize;

    list->values[0].value = POISON;
    list->values[0].previous = 0;
    list->values[0].next = 0;

    for (size_t i = 1; i < listSize; i++) {
        list->values[i].value    =  0;
        list->values[i].previous = -1;
        if (i != listSize - 1) {
            list->values[i].next = (long) i + 1;
        } else {
            list->values[i].next = 0;
        }
    }

    list->free    = 1;
    list->header  = 0;
    list->tail    = 0;

    if (err) *err = listVerify(list);
}

int listVerify(List_t *list) {
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

void listInsert(List_t *list, Elem_t value, size_t index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index + 1 > list->size, INDEX_BIGGER_SIZE);

    // push to empty list
    if (list->tail == 0 && index == 0) {
        size_t nextFree = (size_t) list->values[list->free].next;
        list->values[list->free].value     = value;
        list->values[list->free].next      = 0;
        list->values[list->free].previous  = 0;

        list->tail = list->free;
        list->free = nextFree;

        return;
    }

    CHECK(list->values[index].previous == -1, INDEX_INCORRECT);

    // == push back
    if (list->values[list->tail].previous < (long) index + 1) {
        size_t nextFree = (size_t) list->values[list->free].next;
        list->values[list->free].value     = value;
        list->values[list->free].next      = 0;
        list->values[list->free].previous  = (long) list->tail;

        list->values[list->tail].next      = (long) list->free;

        list->tail = list->free;
        list->free = nextFree;

        return;
    }

    // push after
    size_t nextFree = (size_t) list->values[list->free].next;
    list->values[list->free].value     = value;
    list->values[list->free].next      = list->values[index + 1].next;
    list->values[list->free].previous  = (long) (index + 1);

    list->values[list->values[index + 1].next].previous = (long) list->free;
    list->values[index+1].next  = (long) list->free;

    list->tail = list->free;
    list->free = nextFree;

}

void listDtor(List_t *list, int *err) {
    CHECK(!list, LIST_NULL);

    if (list->values) {
        free(list->values);
    }

    list->header = POISON;
    list->tail   = POISON;
    list->free   = POISON;
    list->size   = POISON;
}

#if _DEBUG
void mprintf(FILE *file, const char *fmt...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(file, fmt, args);
}

void dumpList(List_t *list, int errorCode, const char *fileName, const char *function, int line) {
    mprintf(logFile, "Assertion failed with code %d\n", errorCode);
    mprintf(logFile, "in %s at %s(%d)\n", function, fileName, line);
    if (!list) {
        mprintf(logFile, "List was null\n");
        return;
    }

    mprintf(logFile, "List_t[%p] '%s' at %s at %s(%d)\n", list, list->debugInfo.name, list->debugInfo.createFunc, list->debugInfo.createFile, list->debugInfo.createLine);
    mprintf(logFile, "\theader = %lu\n", list->header);
    mprintf(logFile, "\ttail = %lu\n", list->tail);
    mprintf(logFile, "\tfree = %lu\n", list->free);
    mprintf(logFile, "\tsize = %lu\n", list->size);

    if (!list->values) {
        mprintf(logFile, "Values are null\n");
        return;
    }

    mprintf(logFile, "Values:\n");   
    for (size_t i = 0; i < list->size; i++) {
        mprintf(logFile, "%9d ", list->values[i].value);
    }

    mprintf(logFile, "\nNext:\n");   
    for (size_t i = 0; i < list->size; i++) {
        mprintf(logFile, "%9d ", list->values[i].next);
    }

    mprintf(logFile, "\nPrevious:\n");   
    for (size_t i = 0; i < list->size; i++) {
        mprintf(logFile, "%9d ", list->values[i].previous);
    }
    mprintf(logFile, "\n\n");
}

void closeLogfile(void) {
    fclose(logFile);
}
#endif