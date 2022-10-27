#include "list.h"

#if _DEBUG
FILE *logFile = fopen("logs.txt", "w");
int   onClose = atexit(closeLogfile);
#endif

void _listCtor(List_t *list, size_t listSize, short needLinear, int *err) {
    CHECK(!list, LIST_NULL);

    list->values = (ListElement_t*) calloc(listSize + 1, sizeof(ListElement_t));
    CHECK(!list->values, CANNOT_ALLOC_MEM);
    list->capacity = listSize;

    fillElemList(list->values, listSize, err);

    list->needLinear = needLinear;
    list->free       = 1;
    list->header     = 0;
    list->tail       = 0;
    list->size       = 0;

    if (err) *err = listVerify(list);
}

void fillElemList(ListElement_t *listElems, size_t capacity, int *err) {
    if (!listElems) {
        if (err) *err = LIST_DATA_NULL;
        return;
    }

    listElems[0].value    = POISON;
    listElems[0].previous = 0;
    listElems[0].next     = 0;

    for (size_t i = 1; i < capacity + 1; i++) {
        listElems[i].value    =  POISON;
        listElems[i].previous = -1;
        if (i != capacity - 1) {
            listElems[i].next = (long) i + 1;
        } else {
            listElems[i].next = 0;
        }
    }
}

int listVerify(List_t *list) {
    if (!list)                          return LIST_NULL;
    if (!list->values)                  return LIST_DATA_NULL;
    if (list->header == POISON)         return LIST_HEADER_POISONED;
    if (list->tail == POISON)           return LIST_TAIL_POISONED;
    if (list->free == POISON)           return LIST_FREE_POISONED;
    if (list->size == POISON)           return LIST_SIZE_POISONED;
    if (list->header > list->capacity)  return LIST_HEADER_POISONED;
    if (list->tail   > list->capacity)  return LIST_TAIL_POISONED;
    
    for (size_t i = 1; i < list->capacity; i++) {
        if (list->values[i].value == POISON)    return ELEM_VALUE_POISONED;
        if (list->values[i].next == POISON)     return ELEM_NEXT_POISONED;
        if (list->values[i].previous == POISON) return ELEM_PREV_POISONED;
    }

    return LIST_OK;
}

void _listInsertPhys(List_t *list, Elem_t value, size_t index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity + 1, INDEX_BIGGER_SIZE);

    if (list->capacity - list->size <= 2) {
        size_t newCapacity = list->capacity * RESIZE_COEFFICIENT;
        listResize(list, newCapacity, err);
    }

    // push to empty list
    if (list->tail == 0 && index == 1) {
        size_t nextFree = (size_t) list->values[list->free].next;
        list->values[list->free].value     = value;
        list->values[list->free].next      = 0;
        list->values[list->free].previous  = 0;

        list->tail       = list->free;
        list->free       = nextFree;
        list->linearized = 1;
        list->header     = 1;

        list->size++;

        return;
    }   

    // == push back
    if (list->tail <= index) {
        size_t nextFree = (size_t) list->values[list->free].next;
        list->values[list->free].value     = value;
        list->values[list->free].next      = 0;
        list->values[list->free].previous  = (long) list->tail;

        list->values[list->tail].next      = (long) list->free;

        list->tail       = list->free;
        list->free       = nextFree;

        list->size++;

        return;
    }

    CHECK(list->values[index].previous == -1, INDEX_INCORRECT);
    printf("shit %lu\n", index);

    // push after
    size_t nextFree = (size_t) list->values[list->free].next;
    list->values[list->free].value     = value;
    list->values[list->free].next      = list->values[index].next;
    list->values[list->free].previous  = (long) (index);

    list->values[list->values[index].next].previous = (long) list->free;
    list->values[index]                   .next     = (long) list->free;

    list->tail       = list->free;
    list->free       = nextFree;
    list->linearized = 0;
 
    list->size++;

    if (err) *err = listVerify(list);
}

void listInsert(List_t *list, Elem_t value, size_t index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity, INDEX_BIGGER_SIZE);

    size_t physIndex = logicToPhysics(list, index);
    _listInsertPhys(list, value, physIndex, err);
}

Elem_t _listRemovePhys(List_t *list, size_t index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity, INDEX_BIGGER_SIZE);
    CHECK(list->size == 0, NOTHING_TO_DELETE);
    CHECK(list->values[index].value == POISON, ALREADY_POISON);

    // delete last element in list
    if (list->size == 1) {
        Elem_t returnValue = list->values[index].value;

        list->values[index].value      = POISON;
        list->values[index].next       = (long) list->free;
        list->values[index].previous   = -1;

        list->free       = index;
        list->size       = 0;
        list->tail       = 0;
        list->linearized = 1;
        list->header     = 0;

        return returnValue;
    }

    // delete first element (in non-empty list)
    if (index == 1) {
        Elem_t returnValue = list->values[index].value;

        list->values[index].value      = POISON;
        list->header                   = (size_t) list->values[index].next;
        list->values[index].next       = (long) list->free;
        list->values[index].previous   = -1;

        list->size--;
        list->free   = index;

        return returnValue;
    }

    // pop from back
    if (list->tail == index) {
        Elem_t returnValue = list->values[index].value;

        list->values[index].value = POISON;

        list->values[list->values[index].previous].next     = list->values[index].next;
        list->values[list->values[index].next]    .previous = list->values[index].previous;
        list->values[index]                       .next     = (long) list->free;

        list->tail = (size_t) list->values[index].previous;
        list->values[index].previous   = -1;

        list->size--;
        list->free       = index;
        list->linearized = 0;

        return returnValue;
    }

    // BASA case
    Elem_t returnValue = list->values[index].value;

    list->values[index].value = POISON;

    list->values[list->values[index].previous].next     = list->values[index].next;
    list->values[list->values[index].next]    .previous = list->values[index].previous;
    list->values[index]                       .next     = (long) list->free;
    list->values[index]                       .next     = (long) list->free;

    list->values[index].previous = -1;

    list->size--;
    list->free = index;

    return returnValue;
}

Elem_t listRemove(List_t *list, size_t index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity, INDEX_BIGGER_SIZE);
    CHECK(list->size == 0, NOTHING_TO_DELETE);

    size_t physIndex = logicToPhysics(list, index);
    return _listRemovePhys(list, physIndex, err);
}

[[nodiscard]] size_t logicToPhysics(List_t *list, size_t logicIndex, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(logicIndex > list->capacity, INDEX_INCORRECT);

    if (list->size == 0) return 1;

    size_t pos = list->header;
    if (logicIndex == 0) return pos;

    if (list->linearized) return logicIndex + 1;

    for (size_t i = 0; i < logicIndex; i++) {
        if (list->values[pos].next == 0) {
            return pos;
        }
        pos = (size_t) list->values[pos].next;
    }

    return pos;
}

void listLinearize(List_t *list, int *err) {
    CHECK(!list, LIST_NULL);

    if (list->linearized) return;

    ListElement_t *elements = (ListElement_t *) calloc(list->capacity + 1, sizeof(ListElement_t));
    CHECK(!elements, CANNOT_ALLOC_MEM);

    fillElemList(elements, list->capacity, err);

    size_t oldIndex = list->header;
    for (size_t i = 0; i < list->size; i++) {
        elements[i + 1].value = list->values[oldIndex].value;

        if (i + 1 >= list->size) elements[i + 1].next  = 0;
        else                     elements[i + 1].next  = (long) i + 1;

        if (i == 0) elements[i + 1].previous = 0;
        else        elements[i + 1].previous = (long) i;

        oldIndex = (size_t) list->values[oldIndex].next;
    }

    free(list->values);
    list->values     = elements;
    list->linearized = 1;
    list->free       = list->size + 1;

    if (err) *err = listVerify(list);
}

void listResize(List_t *list, size_t newCapacity, int *err) {
    CHECK(!list, LIST_NULL);

    if (list->needLinear) {
        listLinearize(list, err);
    }

    if (newCapacity < list->capacity) {
        // checking if no sensible data will be deleted
        if (checkForPoisons(list, newCapacity, err)) {
            listRealloc(list, newCapacity, err);
            list->values[newCapacity - 1].next = 0;
        } else {
            if (err) *err = LOSING_DATA;
        }

        return;
    }

    size_t oldCapacity = list->capacity;
    listRealloc(list, newCapacity, err);
    poisonList (list, newCapacity, oldCapacity, err);
}

int checkForPoisons(List_t *list, size_t newCapacity, int *err) {
     CHECK(!list, LIST_NULL);

     size_t goodCounter = 0;
     for (size_t i = list->capacity - 1; i > 0; i--) {
        if (list->values[i].value == POISON) {
            goodCounter++;
        }

        if (goodCounter >  list->capacity - newCapacity) return 1;
     }  

     return 0; 
}

void listRealloc(List_t *list, size_t newCapacity, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(!list->values, LIST_DATA_NULL);

    list->values = (ListElement_t *) realloc(list->values, newCapacity * sizeof(ListElement_t));
    CHECK(!list->values, CANNOT_ALLOC_MEM);

    list->capacity = newCapacity;
}

void poisonList(List_t *list, size_t newCapacity, size_t oldCapacity, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(!list->values, LIST_DATA_NULL);
    CHECK(newCapacity < oldCapacity, INDEX_INCORRECT);

    list->values[list->free].next = (long) oldCapacity;

    for (size_t i = oldCapacity; i < newCapacity; i++) {
        list->values[i].value    = POISON;
        list->values[i].previous = -1;

        if (i == newCapacity - 1) {
            list->values[i].next = 0;
        } else {
            list->values[i].next = (long) i + 1;
        }
    }
}

void listDtor(List_t *list, int *err) {
    CHECK(!list, LIST_NULL);

    if (list->values) {
        free(list->values);
    }

    list->header     = POISON;
    list->tail       = POISON;
    list->free       = POISON;
    list->size       = POISON;
    list->capacity   = POISON;
    list->linearized = 1;
}

void visualGraph(List_t *list, const char *outputName) {
    if (!list) return;

    FILE *tempFile = fopen("temp.dot", "w");
    if (!tempFile) return;

    mprintf(tempFile, "digraph structs {\n");
    mprintf(tempFile, "\trankdir=LR;\n");
    for (size_t i = 0; i < list->capacity; i++) {
        mprintf(
                    tempFile, 
                    "\tlabel%lu[shape=record, style=rounded, label=\"{%d | {%ld | %ld} }\"];\n", 
                    i, 
                    list->values[i].value, 
                    list->values[i].next,
                    list->values[i].previous
                );
    }

    mprintf(tempFile, "}");

    fclose(tempFile);

    char command1[MAX_STR_LENGTH] = "dot -Tsvg temp.dot > ";
    strcat(command1, outputName);
    system(command1);
    
    char command2[MAX_STR_LENGTH] = "xdg-open ";
    strcat(command2, outputName);
    system(command2);
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
    mprintf(logFile, "\tcapacity = %lu\n", list->capacity);
    mprintf(logFile, "\tlinearized = %d\n", list->linearized);

    if (!list->values) {
        mprintf(logFile, "Values are null\n");
        return;
    }

    mprintf(logFile, "Values:\n");   
    for (size_t i = 0; i < list->capacity; i++) {
        mprintf(logFile, "%9d ", list->values[i].value);
    }

    mprintf(logFile, "\nNext:\n");   
    for (size_t i = 0; i < list->capacity; i++) {
        mprintf(logFile, "%9d ", list->values[i].next);
    }

    mprintf(logFile, "\nPrevious:\n");   
    for (size_t i = 0; i < list->capacity; i++) {
        mprintf(logFile, "%9d ", list->values[i].previous);
    }
    mprintf(logFile, "\n\n");
}

void closeLogfile(void) {
    fclose(logFile);
}
#endif