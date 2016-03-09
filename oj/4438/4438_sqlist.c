#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#define null 0
#define MAXSIZE 50

struct strlnode
{
    int data;
    struct strlnode *plast;
    struct strlnode *pnext;
};

void create(struct strlnode **p, int x)
{
    struct strlnode *q;

    q = (struct strlnode *)malloc(sizeof(struct strlnode));
    q->data = x;
    q->plast = null;
    q->pnext = null;

    *p = q;

    return;
}

void insertnode(struct strlnode **p, int i, int x)
{
    if (i < 0)
        return;
    struct strlnode dummy = {-1, NULL, *p}, *q = &dummy;
    while (i && q) {
        q = q->pnext;
        --i;
    }
    if (!q)
        return;
    struct strlnode *n;
    create(&n, x);
    if (q != &dummy)
        n->plast = q;
    n->pnext = q->pnext;
    if (q->pnext)
        q->pnext->plast = n;
    q->pnext = n;
    *p = dummy.pnext;
}

void deletenode(struct strlnode **p, int i) 
{
    if (i < 0)
        return;
    struct strlnode dummy = {-1, NULL, *p}, *q = &dummy;
    while (i && q) {
        q = q->pnext;
        --i;
    }
    if (!q)
        return;
    struct strlnode *d = q->pnext;
    q->pnext = d->pnext;
    d->pnext->plast = q;
    free(d);
}

int getnodenum(struct strlnode **p)
{
    int nodenum = 0;
    struct strlnode *q;
    for (q = *p; q; q = q->pnext)
        ++nodenum;
    return nodenum;
}

void copysqlist(struct strlnode **dst, struct strlnode **src)
{
    struct strlnode *d = *dst, *s = *src, dummy = {-1, NULL, NULL}, *pos = &dummy;
    for (; s; s = s->pnext) {
        struct strlnode *n;
        create(&n, s->data);
        n->plast = pos;
        pos->pnext = n;
        pos = pos->pnext;
    }
    *dst = dummy.pnext;
}

void bignumberplus(struct strlnode **plus, struct strlnode **p, struct strlnode **q) 
{
    struct strlnode *pptr = *p, *qptr = *q;
    *plus = NULL;
    if (pptr && qptr) {
        while (pptr->pnext)
            pptr = pptr->pnext;
        while (qptr->pnext)
            qptr = qptr->pnext;
        int carry = 0;
        while (pptr && qptr) {
            int tmp = carry + pptr->data + qptr->data;
            carry = tmp / 10;
            insertnode(plus, 0, tmp%10);
            pptr = pptr->plast;
            qptr = qptr->plast;
        }
        while (pptr) {
            int tmp = carry + pptr->data;
            carry = tmp / 10;
            insertnode(plus, 0, tmp%10);
            pptr = pptr->plast;
        }
        while (qptr) {
            int tmp = carry + qptr->data;
            carry = tmp / 10;
            insertnode(plus, 0, tmp%10);
            qptr = qptr->plast;
        }
        if (carry)
            insertnode(plus, 0, carry);
    } else if (pptr) {
        copysqlist(plus, p);
    } else if (qptr) {
        copysqlist(plus, q);
    }
}


void readtolnode(struct strlnode **p, int *a, int size)  
{
    int j = 0;
    int data = 0;
    struct strlnode *s = *p;

    s->data = *(a + (size-1));

    for(j = 2; j < (size+1); j++)
    {
        data = *(a + (size-j));
        insertnode(p, 0, data);
    }

    return;
}


void writetosqlist(int *a, struct strlnode *p)  
{
    int j = 0;
    struct strlnode *s = p;

    while(s != null)
    {
        *(a + j) = s->data;
        s = s->pnext;
        j++;
    }

    return;
}

void TestCase01_1()
{
    struct strlnode *p;

    create(&p, 1);

    assert(p != null);
    assert(p->data == 1);
    assert(p->plast == null);
    assert(p->pnext == null);
}
void TestCase02_1()
{
    struct strlnode *p;

    create(&p, 1);
    insertnode(&p, 1, 2);
    insertnode(&p, 2, 3);
    insertnode(&p, 3, 4);

    insertnode(&p, 2, 5);

    assert(p->data == 1);
    p = p->pnext;
    assert(p->data == 2);
    p = p->pnext;
    assert(p->data == 5);
    p = p->pnext;
    assert(p->data == 3);
    p = p->pnext;
    assert(p->data == 4);
    assert(p->pnext == null);

    p = p->plast;
    assert(p->data == 3);
    p = p->plast;
    assert(p->data == 5);
    p = p->plast;
    assert(p->data == 2);
    p = p->plast;
    assert(p->data == 1);
    assert(p->plast == null);

}
void TestCase03_1()
{
    struct strlnode *p;

    create(&p, 1);
    insertnode(&p, 1, 2);
    insertnode(&p, 2, 3);
    insertnode(&p, 3, 4);

    deletenode(&p, 2);

    assert(p->data == 1);
    p = p->pnext;
    assert(p->data == 2);
    p = p->pnext;
    assert(p->data == 4);
    assert(p->pnext == null);

    p = p->plast;
    assert(p->data == 2);
    p = p->plast;
    assert(p->data == 1);
    assert(p->plast == null);
}
void TestCase04_1()
{
    struct strlnode *p;
    int num = 0;

    create(&p, 1);
    insertnode(&p, 1, 2);
    insertnode(&p, 2, 3);
    insertnode(&p, 3, 4);

    num = getnodenum(&p);

    assert(num == 4);
}
void TestCase05_1()
{
    struct strlnode *plus;
    struct strlnode *p;
    struct strlnode *q;
    int a[] = {4,8,6,3,0,2,7,9,3,1,0,4,2,1,3,5,2,6};
    int b[] = {9,2,0,8,6,7,8,3,1,0,3,6,9,2,0,1,6,8};
    int j = 0;

    create(&plus, 0);
    create(&p, 0);
    create(&q, 0);

    readtolnode(&p, a, 18);
    readtolnode(&q, b, 18);

    bignumberplus(&plus, &p, &q);

    int result[19];
    writetosqlist(result, plus);
    int c[] = {1,4,0,7,1,7,0,6,2,4,1,4,1,1,3,3,6,9,4};

    for(j = 0; j < 19; j++)
    {
        // printf("%d ", result[j]);
        assert(result[j] == c[j]);
    }
    // printf("\n");
}

void TestCase05_2()
{
    struct strlnode *plus;
    struct strlnode *p;
    struct strlnode *q;
    int a[] = {4,8,6,3,0,2,7,9,3,1,0,4,2,1,3,5,2,6};
    int b[] = {7,6,0,9,3,4,0,1,9,2,0,8,6,7,8,3,1,0,3,6,9,2,0,1,6,8};
    int j = 0;

    create(&plus, 0);
    create(&p, 0);
    create(&q, 0);

    readtolnode(&p, a, 18);
    readtolnode(&q, b, 26);

    bignumberplus(&plus, &p, &q);

    int result[26];
    writetosqlist(result, plus);
    int c[] = {7,6,0,9,3,4,0,2,4,0,7,1,7,0,6,2,4,1,4,1,1,3,3,6,9,4};

    for(j = 0; j < 26; j++)
    {
        assert(result[j] == c[j]);
    }
}

void TestCase05_3()
{
    struct strlnode *plus;
    struct strlnode *p;
    struct strlnode *q;
    int a[] = {7,6,0,9,3,4,0,1,9,2,0,8,6,7,8,3,1,0,3,6,9,2,0,1,6,8};
    int b[] = {4,8,6,3,0,2,7,9,3,1,0,4,2,1,3,5,2,6};
    int j = 0;

    create(&plus, 0);
    create(&p, 0);
    create(&q, 0);

    readtolnode(&p, a, 26);
    readtolnode(&q, b, 18);

    bignumberplus(&plus, &p, &q);

    int result[26];
    writetosqlist(result, plus);
    int c[] = {7,6,0,9,3,4,0,2,4,0,7,1,7,0,6,2,4,1,4,1,1,3,3,6,9,4};

    for(j = 0; j < 26; j++)
    {
        assert(result[j] == c[j]);
    }
}

int main(void)
{
    TestCase01_1();
    TestCase02_1();
    TestCase03_1();
    TestCase04_1();
    TestCase05_1();
    TestCase05_2();
    TestCase05_3();
    return 0;
}
