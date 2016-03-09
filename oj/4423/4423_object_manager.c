#include "4423_object_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

obj_node_t head = {WILDCARD, WILDCARD, WILDCARD, NULL};

int AddObject (unsigned int key1, unsigned int key2, unsigned int key3)
{
    if (IsObjectExist(key1, key2, key3))
        return -1;
    obj_node_t *n = (obj_node_t *)malloc(sizeof(obj_node_t));
    n->key1 = key1;
    n->key2 = key2;
    n->key3 = key3;
    n->next = head.next;
    head.next = n;
    return 0;
}

void DeleteObject (unsigned int key1, unsigned int key2, unsigned int key3)
{
    obj_node_t *prev = &head, *p = prev->next;
    while (p) {
        if ((p->key1 == key1 || key1 == WILDCARD)
                && (p->key2 == key2 || key2 == WILDCARD)
                && (p->key3 == key3 || key3 == WILDCARD)) {
            prev->next = prev->next->next;
            free(p);
        } else
            prev = prev->next;
        p = prev->next;
    }
}

int IsObjectExist (unsigned int key1, unsigned int key2, unsigned int key3)
{
    obj_node_t *p = head.next;
    while (p) {
        if (p->key1 == key1 && p->key2 == key2 && p->key3 == key3)
            return 1;
        p = p->next;
    }
    return 0;
}

void Clear(void)
{
    obj_node_t *p = head.next;
    while (p) {
        head.next = p->next;
        free(p);
        p = head.next;
    }
}

int main(void)
{
    assert(0 == AddObject(1, 2, 3));
    assert(0 == AddObject(1, 2, 4));
    assert(0 == AddObject(1, 5, 1));
    assert(0 == AddObject(1, 5, 3));
    assert(0 == AddObject(2, 3, 4));
    assert(0 == AddObject(2, 3, 5));
    assert(0 == AddObject(2, 4, 5));
    assert(0 == AddObject(2, 3, 6));

    assert(-1 == AddObject(2, 3, 6));

    int result =IsObjectExist(1, 5, 3);
    assert(1 == result);

    DeleteObject(2, 0xFFFFFFFF, 5);
    result = IsObjectExist(2, 3, 5);
    assert(0 == result);
    return 0;
}
