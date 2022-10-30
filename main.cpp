#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    listCtor(&list, 2, 1, &err);
    visualGraph(&list, "Initialized list");

    listPushBack(&list, 7);
    visualGraph(&list, "Push back 7");

    long nineInd = listPushBack(&list, 9);
    visualGraph(&list, "Push back 9");

    listInsert(&list, 1, 2);
    visualGraph(&list, "Insert 1 on second position (logical)");

    listPushFront(&list, 11);
    visualGraph(&list, "Push front 11");

    _listInsertPhys(&list, 12, nineInd);
    visualGraph(&list, "Inserted 12 after 9 (phys index)");

    listLinearize(&list);
    visualGraph(&list, "Linearize list");

    listRemove(&list, 0);
    visualGraph(&list, "Remove on 0 (logic) index");

    listRemove(&list, 3);
    visualGraph(&list, "Remove on 3 (last logic) index");

    listRemove(&list, 1);
    visualGraph(&list, "Remove on 1 (logic) index");

    listLinearize(&list);
    visualGraph(&list, "Linearize list");

    listDtor(&list, &err);

    return 0;
}