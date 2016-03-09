#ifndef __OBJMGT_H__
#define __OBJMGT_H__

#define WILDCARD 0xFFFFFFFF

typedef struct obj_node_t {
    unsigned int key1, key2, key3;
    struct obj_node_t *next;
} obj_node_t;

extern obj_node_t head;

int  AddObject (unsigned int key1, unsigned int key2, unsigned int key3);
void  DeleteObject (unsigned int key1, unsigned int key2, unsigned int key3);
int  IsObjectExist (unsigned int key1, unsigned int key2, unsigned int key3);
void Clear(void);
#endif
