#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    listCtor(&list, MAX_LIST_LENGTH, &err);
    listInsert(&list, 5, 0);
    DUMP(&list, 0);
    listInsert(&list, 6, 1);
    DUMP(&list, 0);
    listDtor(&list, &err);
}