#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    listCtor(&list, MAX_LIST_LENGTH, &err);
    printf("%ld ", list.values[2].next);
    listDtor(&list, &err);
}