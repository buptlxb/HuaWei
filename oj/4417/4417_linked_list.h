#ifndef __OJ_H__
#define __OJ_H__

typedef struct strNode  
{
    int data;
    struct strNode * pstrNext;
}strNode;

typedef struct hash_node_t {
    int key, cnt;
    struct hash_node_t *next;
} hash_node_t;


int iChanProcess(strNode * pstrIn,strNode * pstrOut);
void vFreeChan(strNode * pstrChan);

#endif
