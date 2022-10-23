#include <stdio.h>

#include "list.h"


size_t MAX_LIST_LENGTH = 256;

int main() {
    List_t list = {};
    int err = 0;
    ListCtor(&list, MAX_LIST_LENGTH, &err);
    printf("%d ", list.values[2].next);
    ListDtor(&list, &err);
}