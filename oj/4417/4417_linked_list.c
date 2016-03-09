#include <stdlib.h>
#include <stdio.h>
#include "4417_linked_list.h"
#include <limits.h>
#include <assert.h>


int iChanProcess(strNode * pstrIn,strNode * pstrOut)
{
    if (!pstrIn) {
        return 0;
    }
    strNode *p = pstrIn;

    int cnt = 0, min = INT_MAX, max = INT_MIN;
    while (p) {
        ++cnt;
        if (p->data < min)
            min = p->data;
        if (p->data > max)
            max = p->data;
        p = p->pstrNext;
    }
    int step = (max - min) / cnt + 1;
    int num = (max-min) / step + 1;
    hash_node_t *ht = (hash_node_t*)malloc(num * sizeof(hash_node_t));
    for (p = pstrIn; p; p = p->pstrNext) {
        int idx = (p->data - min) / step;
        assert(idx < num);
        hash_node_t *bucket = &ht[idx];
        while (bucket->next && bucket->next->key != p->data) {
            bucket = bucket->next;
        }
        if (bucket->next) {
            bucket->next->cnt += 1;
        } else {
            bucket->next = (hash_node_t *)malloc(sizeof(hash_node_t));
            bucket->next->key = p->data;
            bucket->next->cnt = 1;
            bucket->next->next = NULL;
        }
    }
    strNode dummy = {-1, NULL}, *pos = &dummy;
    for (p = pstrIn; p; ) {
        int idx = (p->data - min) / step;
        hash_node_t *bucket = &ht[idx];
        while (bucket->next && bucket->next->key != p->data) {
            bucket = bucket->next;
        }
        assert(bucket->next);
        strNode *n = p;
        p = n->pstrNext;
        if (bucket->next->cnt == 1) {
            strNode *x = (strNode *)malloc(sizeof(strNode));
            *x = *n;
            x->pstrNext = pos->pstrNext;
            pos->pstrNext = x;
        } else
            free(n);
    }
    if (pos->pstrNext)
        *pstrOut = *(pos->pstrNext);

    int i;
    for (i = 0; i < num; ++i) {
        hash_node_t *bucket = &ht[i];
        while (bucket->next) {
            hash_node_t *n = bucket->next;
            bucket->next = n->next;
            free(n);
        }
    }
    free(ht);
    return 0;
}

void vFreeChan(strNode * pstrChan)
{
    while (pstrChan) {
        strNode *n = pstrChan;
        pstrChan = n->pstrNext;
        free(n);
    }
}

int main(void)
{
    strNode *n1 = (strNode *)malloc(sizeof(strNode));
    strNode *n2 = (strNode *)malloc(sizeof(strNode));
    strNode *n3 = (strNode *)malloc(sizeof(strNode));
    strNode *n4 = (strNode *)malloc(sizeof(strNode));
    strNode *n5 = (strNode *)malloc(sizeof(strNode));
    strNode *n6 = (strNode *)malloc(sizeof(strNode));
    strNode *n7 = (strNode *)malloc(sizeof(strNode));
    n1->pstrNext = n2;
    n2->pstrNext = n3;
    n3->pstrNext = n4;
    n4->pstrNext = n5;
    n5->pstrNext = n6;
    n6->pstrNext = n7;
    n1->data = 6;
    n2->data = 7;
    n3->data = 8;
    n4->data = 8;
    n5->data = 9;
    n6->data = 10;
    n7->data = 6;
    strNode *out = (strNode *)malloc(sizeof(strNode));
    iChanProcess(n1, out);
    while (out) {
        printf("%d\n", out->data);
        out = out->pstrNext;
    }
    vFreeChan(out);
    out = (strNode *)malloc(sizeof(strNode));
    iChanProcess(NULL, out);
    return 0;
}
