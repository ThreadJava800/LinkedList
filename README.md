# Linked list
This is my (cache-friendly) realization of doubly-linked list.

**Mention: this is not classic realization of linked list with pointers (but you can check out it [there](https://github.com/ThreadJava800/ClassicList))**

My list contains indexes to previous and next values. (You can check out my list structure [here](https://github.com/ThreadJava800/LinkedList/blob/main/list.h#L61)).

First value in list is "technical": it contains info about indexes of last and first values in list. This trick helps me to make my list circled and write less if-else constructions.

I also made some utils for debug. You can use basic text dump (to log file) or its visualised html version (images are made with graphviz library).

# Functionality
My list has several functions:

- pushBack (List_t*, Elem_t) - pushes value to tail of list
- pushFront(List_t*, Elem_t) - pushes value to head of list
- insert   (List_t*, Elem_t, long) - pushes value to logical index of list
- insertPhys (List_t*, Elem_t, long) - pushes value to physical index of list *(works o(1))*
- remove (List_t*, Elem_t, long) - removes value from logical index of list
- removePhys (List_t*, Elem_t, long) - removes value from physical index of list *(works o(1))*
- logicToPhysics (List_t*, long) - return physical index from logic index
- listLinearize(List_t*) - "sorts" list in its logical order **(caution!) you lose all physical indexes**
- listResize(List_t*, long) - resizes list (list automatically resizes up if needed). This function also checks if you are not losing data (when resize down).
- visualGraph(List_t*, [optional]char* comment) and DUMP(List_t*) - graphic and text dumps.