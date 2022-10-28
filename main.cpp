#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    listCtor(&list, 10, 0, &err);
    DUMP(&list, 0);
    // visualGraph(&list, "test.png");

    printf("%d ", logicToPhysics(&list, 0));
    listInsert(&list, 1, 1);
    printf("%d ", logicToPhysics(&list, 0));
    DUMP(&list, 0);
    listInsert(&list, 2, 1);
    DUMP(&list, 0);
    listInsert(&list, 3, 1);
    DUMP(&list, 0);
    // listInsert(&list, 4, 1);
    // DUMP(&list, 0);
    // listInsert(&list, 5, 1);
    // DUMP(&list, 0);
    // listRemove(&list, 1);
    // DUMP(&list, 0);
    // listRemove(&list, 0);
    // DUMP(&list, 0);
    // // DUMP(&list, 0);
    // listResize(&list, 8);
    // DUMP(&list, 0);
    // //listLinearize(&list);

    listDtor(&list, &err);

    return 0;
}