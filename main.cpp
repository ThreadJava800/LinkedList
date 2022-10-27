#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    listCtor(&list, 10, &err);

    listInsert(&list, 1, 0);
    listInsert(&list, 2, 1);
    listInsert(&list, 3, 1);
    listInsert(&list, 4, 1);
    listInsert(&list, 5, 1);
    listRemove(&list, 1);
    DUMP(&list, 0);
    listRemove(&list, 1);
    DUMP(&list, 0);
    listLinearize(&list);
    DUMP(&list, 0);

    listDtor(&list, &err);

    return 0;
}