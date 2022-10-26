#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    listCtor(&list, 10, &err);
    _listInsertPhys(&list, 5, 1);
    DUMP(&list, 0);
    _listInsertPhys(&list, 6, 2);
    DUMP(&list, 0);
    _listInsertPhys(&list, 7, 3);
    DUMP(&list, 0);
    _listRemovePhys(&list, 3);
    DUMP(&list, 0);
    // _listInsertPhys(&list, 6, 1);
    // DUMP(&list, 0);
    // _listInsertPhys(&list, 7, 2);
    // DUMP(&list, 0);
    // _listInsertPhys(&list, 8, 3);
    // DUMP(&list, 0);
    // _listInsertPhys(&list, 9, 2);
    // DUMP(&list, 0);

    listDtor(&list, &err);

    return 0;
}