#include "list.h"
#include "colors.h"

#if _DEBUG
FILE *logFile = fopen("logs.txt", "w");
int   onClose = atexit(closeLogfile);
#endif

void _listCtor(List_t *list, long listSize, short needLinear, int *err) {
    CHECK(!list, LIST_NULL);

    list->values = (ListElement_t*) calloc((size_t) listSize + 1, sizeof(ListElement_t));
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

void fillElemList(ListElement_t *listElems, long capacity, int *err) {
    if (!listElems) {
        if (err) *err = LIST_DATA_NULL;
        return;
    }

    listElems[0].value    = POISON;
    listElems[0].previous = 0;
    listElems[0].next     = 0;

    for (long i = 1; i < capacity + 1; i++) {
        listElems[i].value    = POISON;
        listElems[i].previous = -1;
        if (i != capacity - 1) {
            listElems[i].next =  i + 1;
        } else {
            listElems[i].next = 0;
        }
    }
}

int listVerify(List_t *list) {
    if (!list)                          return LIST_NULL;
    if (!list->values)                  return LIST_DATA_NULL;
    if (list->header == POISON)         return LIST_HEADER_POISONED;
    if (list->header < 0)               return LIST_HEADER_POISONED;
    if (list->tail == POISON)           return LIST_TAIL_POISONED;
    if (list->tail < 0)                 return LIST_TAIL_POISONED;
    if (list->free == POISON)           return LIST_FREE_POISONED;
    if (list->free < 0)                 return LIST_FREE_POISONED;
    if (list->size == POISON)           return LIST_SIZE_POISONED;
    if (list->size < 0)                 return LIST_SIZE_POISONED;
    if (list->header > list->capacity)  return LIST_HEADER_POISONED;
    if (list->tail   > list->capacity)  return LIST_TAIL_POISONED;
    
    for (long i = 1; i < list->capacity; i++) {
        if (list->values[i].value == POISON)    return ELEM_VALUE_POISONED;
        if (list->values[i].next == POISON)     return ELEM_NEXT_POISONED;
        if (list->values[i].previous == POISON) return ELEM_PREV_POISONED;
    }

    return LIST_OK;
}

long _listInsertPhys(List_t *list, Elem_t value, long index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity + 1, INDEX_BIGGER_SIZE);

    if (list->capacity - list->size <= 2) {
        long newCapacity = list->capacity * RESIZE_COEFFICIENT;
        listResize(list, newCapacity, err);
    }

    long nextFree =  list->values[list->free].next;
    long pushInd  = list->free;

    // push to empty list
    // список, через отдельные блоки и указатели (классический)
    // каждый сделан отдельным каллоком, с указателем на некст и прев
    // список структур
    if (list->tail == 0 && index == 1) {
        list->values[list->free].value     = value;
        list->values[list->free].next      = 0;
        list->values[list->free].previous  = 0;

        list->tail       = list->free;
        list->free       = nextFree;
        list->linearized = 1;
        list->header     = 1;

        list->size++;

        return pushInd;
    }   

    // == push back
    if (list->tail <= index) {
        list->values[list->free].value     = value;
        list->values[list->free].next      = 0;
        list->values[list->free].previous  = list->tail;

        list->values[list->tail].next      = list->free;

        list->tail       = list->free;
        list->free       = nextFree;

        list->size++;

        return pushInd;
    }

    CHECK(list->values[index].previous == -1, INDEX_INCORRECT);

    // push after
    list->values[list->free].value     = value;
    list->values[list->free].next      = list->values[index].next;
    list->values[list->free].previous  = index;

    list->values[list->values[index].next].previous = list->free;
    list->values[index]                   .next     = list->free;

    list->free       = nextFree;
    list->linearized = 0;
 
    list->size++;

    if (err) *err = listVerify(list);

    return pushInd;
}

long listInsert(List_t *list, Elem_t value, long index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity, INDEX_BIGGER_SIZE);

    long physIndex = logicToPhysics(list, index);
    return _listInsertPhys(list, value, physIndex, err);
}

Elem_t _listRemovePhys(List_t *list, long index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity, INDEX_BIGGER_SIZE);
    CHECK(list->size == 0, NOTHING_TO_DELETE);
    CHECK(list->values[index].value == POISON, ALREADY_POISON);

    // delete last element in list
    if (list->size == 1) {
        Elem_t returnValue = list->values[index].value;

        list->values[index].value      = POISON;
        list->values[index].next       = list->free;
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
        list->header                   = list->values[index].next;
        list->values[index].next       = list->free;
        list->values[index].previous   = -1;

        list->size--;
        list->free   = index;

        return returnValue;
    }

    // pop from back
    if (list->tail == index) {
        printf("int");
        Elem_t returnValue = list->values[index].value;

        list->values[index].value = POISON;

        list->values[list->values[index].previous].next     = list->values[index].next;
        list->values[list->values[index].next]    .previous = list->values[index].previous;
        list->values[index]                       .next     = list->free;

        list->tail =  list->values[index].previous;
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
    list->values[index]                       .next     = list->free;
    list->values[index]                       .next     = list->free;

    list->values[index].previous = -1;

    list->size--;
    list->free = index;

    return returnValue;
}

Elem_t listRemove(List_t *list, long index, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(index > list->capacity, INDEX_BIGGER_SIZE);
    CHECK(list->size == 0, NOTHING_TO_DELETE);

    long physIndex = logicToPhysics(list, index);
    return _listRemovePhys(list, physIndex, err);
}

[[nodiscard]] long logicToPhysics(List_t *list, long logicIndex, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(logicIndex > list->capacity, INDEX_INCORRECT);

    if (list->size == 0) return 1;

    long pos = list->header;
    if (logicIndex == 0) return pos;

    if (list->linearized) return logicIndex + 1;

    for (long i = 0; i < logicIndex; i++) {
        if (list->values[pos].next == 0) {
            return pos;
        }
        pos =  list->values[pos].next;
    }

    return pos;
}

void listLinearize(List_t *list, int *err) {
    CHECK(!list, LIST_NULL);

    if (list->linearized) return;

    ListElement_t *elements = (ListElement_t *) calloc((size_t) list->capacity + 1, sizeof(ListElement_t));
    CHECK(!elements, CANNOT_ALLOC_MEM);

    fillElemList(elements, list->capacity, err);

    long oldIndex = list->header;
    for (long i = 0; i < list->size; i++) {
        elements[i + 1].value = list->values[oldIndex].value;

        if (i + 1 >= list->size) elements[i + 1].next  = 0;
        else                     elements[i + 1].next  = i + 2;

        if (i == 0) elements[i + 1].previous = 0;
        else        elements[i + 1].previous =  i;

        oldIndex =  list->values[oldIndex].next;
    }

    free(list->values);
    list->values     = elements;
    list->linearized = 1;
    list->free       = list->size + 1;

    if (err) *err = listVerify(list);
}

void listResize(List_t *list, long newCapacity, int *err) {
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

    long oldCapacity = list->capacity;
    listRealloc(list, newCapacity, err);
    poisonList (list, newCapacity, oldCapacity, err);
}

int checkForPoisons(List_t *list, long newCapacity, int *err) {
     CHECK(!list, LIST_NULL);

     long goodCounter = 0;
     for (long i = list->capacity - 1; i > 0; i--) {
        if (list->values[i].value == POISON) {
            goodCounter++;
        }

        if (goodCounter >  list->capacity - newCapacity) return 1;
     }  

     return 0; 
}

void listRealloc(List_t *list, long newCapacity, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(!list->values, LIST_DATA_NULL);

    list->values = (ListElement_t *) realloc(list->values, (size_t) newCapacity * sizeof(ListElement_t));
    CHECK(!list->values, CANNOT_ALLOC_MEM);

    list->capacity = newCapacity;
}

void poisonList(List_t *list, long newCapacity, long oldCapacity, int *err) {
    CHECK(!list, LIST_NULL);
    CHECK(!list->values, LIST_DATA_NULL);
    CHECK(newCapacity < oldCapacity, INDEX_INCORRECT);

    list->values[list->free].next =  oldCapacity;

    for (long i = oldCapacity; i < newCapacity; i++) {
        list->values[i].value    = POISON;
        list->values[i].previous = -1;

        if (i == newCapacity - 1) {
            list->values[i].next = 0;
        } else {
            list->values[i].next =  i + 1;
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

    mprintf(
                tempFile, 
                "\tinfo[shape=record, style=\"rounded, filled\", fillcolor=\"#d0d1f2\", label=\"{{header: %ld | tail: %ld | free: %ld} | \
                    {size: %ld | cap: %ld} | {linearized: %d |needLinear: %d} }\"];\n", 
                list->header,
                list->tail,
                list->free,
                list->size,
                list->capacity,
                list->linearized,
                list->needLinear
            );

    for (long i = 0; i < list->capacity; i++) {
        char color[MAX_COMMAND_LENGTH];
        if (i == 0) {
            strcpy(color, NULL_BLOCK);
        } else if (list->values[i].value == POISON) {
            strcpy(color, FREE_BLOCK);
        } else {
            strcpy(color, WORK_BLOCK);
        }

        mprintf(
                    tempFile, 
                    "\tlabel%ld[shape=record, style=\"rounded, filled\", fillcolor=\"%s\", label=\"{val: %d | {n: %ld | p: %ld} }\"];\n", 
                    i, 
                    color,
                    list->values[i].value, 
                    list->values[i].next,
                    list->values[i].previous
                );
    }

    for (long i = 0; i < list->capacity - 1; i++) {
        mprintf(tempFile, "\tlabel%ld->label%ld [color=\"%s\", style=\"dashed\",arrowhead=\"none\"]", i, i + 1, PHYS_LINK);
    }

    // from 0 to 0
    mprintf(tempFile, "\tlabel0->label0 [dir=both, color=\"red:blue\"]\n");

    long index = list->header;
    for (long i = 0; i < list->size; i++) {
        long nextIndex =  list->values[index].next;
        long prevIndex =  list->values[index].previous;
        if (nextIndex == 0) break;

        mprintf(tempFile, "\tlabel%ld->label%ld [color=\"red\"]\n", index, nextIndex);
        if (prevIndex) mprintf(tempFile, "\tlabel%ld->label%ld [color=\"blue\"]\n", index, prevIndex);
        index = nextIndex;
    }

    mprintf(tempFile, "\tlabel%ld->label%ld [color=\"blue\"]\n", list->tail, list->values[list->tail].previous);

    index = list->free;
    for (long i = 0; i < list->capacity - list->size; i++) {
        long nextIndex =  list->values[index].next;
        if (nextIndex == 0) break;

        mprintf(tempFile, "\tlabel%ld->label%ld [color=\"#038c61\"]\n", index, nextIndex);
        index = nextIndex;
    }

    mprintf(tempFile, "}");

    fclose(tempFile);

    char command[MAX_COMMAND_LENGTH] = "dot -Tsvg temp.dot > ";
    strcat(command, outputName);
    system(command);
    
    strcpy(command, "xdg-open ");
    strcat(command, outputName);
    system(command);
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
    mprintf(logFile, "\theader = %ld\n", list->header);
    mprintf(logFile, "\ttail = %ld\n", list->tail);
    mprintf(logFile, "\tfree = %ld\n", list->free);
    mprintf(logFile, "\tsize = %ld\n", list->size);
    mprintf(logFile, "\tcapacity = %ld\n", list->capacity);
    mprintf(logFile, "\tlinearized = %d\n", list->linearized);

    if (!list->values) {
        mprintf(logFile, "Values are null\n");
        return;
    }

    mprintf(logFile, "Values:\n");   
    for (long i = 0; i < list->capacity; i++) {
        mprintf(logFile, "%9d ", list->values[i].value);
    }

    mprintf(logFile, "\nNext:\n");   
    for (long i = 0; i < list->capacity; i++) {
        mprintf(logFile, "%9d ", list->values[i].next);
    }

    mprintf(logFile, "\nPrevious:\n");   
    for (long i = 0; i < list->capacity; i++) {
        mprintf(logFile, "%9d ", list->values[i].previous);
    }
    mprintf(logFile, "\n\n");
}

void closeLogfile(void) {
    fclose(logFile);
}
#endif