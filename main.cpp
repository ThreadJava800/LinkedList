#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    listCtor(&list, 2, 1, &err);
    DUMP(&list, 0);

    listPushFront(&list, 1);
    DUMP(&list, 0);
    listPushFront(&list, 2);
    DUMP(&list, 0);
    listPushFront(&list, 3);
    DUMP(&list, 0);
    listPushBack(&list, 4);
    DUMP(&list, 0);
    listPushBack(&list, 5);
    DUMP(&list, 0);
    listPushBack(&list, 6);
    DUMP(&list, 0);
    visualGraph(&list, "WOW");
    visualGraph(&list, "HAHA");

    listDtor(&list, &err);

    return 0;
}