#include <stdlib.h>
#include <assert.h>

#define MAXSIZE 5

struct strmatrix
{
    int rownum; 
    int columnnum; 
    int matrix[MAXSIZE][MAXSIZE];
};

void setnull(struct strmatrix *p)  
{
    int i, j;
    for (i = 0; i < p->rownum; ++i) {
        for (j = 0; j < p->columnnum; ++j) {
            p->matrix[i][j] = 0;
        }
    }
    p->rownum = 0;
    p->columnnum = 0;
}

int getelementnum(struct strmatrix *p)  
{
    return p->rownum * p->columnnum;
}

int getxnum(struct strmatrix *p, int x) 
{
    int num = 0;
    int i, j;
    for (i = 0; i < p->rownum; ++i) {
        for (j = 0; j < p->columnnum; ++j) {
            if (p->matrix[i][j] == x)
                ++num;
        }
    }
    return num;
}

void insertrow(struct strmatrix *p,int i,int *x) 
{
   if (i < 0 || i > p->rownum || p->rownum == MAXSIZE)
       return;
   int j, k;
   for (j = p->rownum ; j > i; --j) {
       for (k = 0; k < p->columnnum; ++k) {
           p->matrix[j][k] = p->matrix[j-1][k];
       }
   }
   for (k = 0; k < p->columnnum; ++k) {
       p->matrix[i][k] = x[k];
   }
   p->rownum += 1;
}

void insertcolumn(struct strmatrix *p,int i,int *x) 
{
    if (i < 0 || i > p->columnnum || p->columnnum == MAXSIZE)
        return;
    int j, k;
    for (j = p->columnnum; j > i; --j) {
        for (k = 0; k < p->rownum; ++k) {
            p->matrix[k][j] = p->matrix[k][j-1];
        }
    }
    for (k = 0; k < p->rownum; ++k) {
        p->matrix[k][i] = x[k];
    }
    p->columnnum +=1;
}

void matrixtrans(struct strmatrix *p) 
{
    if (p->rownum != p->columnnum)
        return;
    int i, j;
    for (i = 0; i < p->rownum; ++i) {
        for (j = 0; j < i; ++j) {
            int tmp = p->matrix[i][j];
            p->matrix[i][j] = p->matrix[j][i];
            p->matrix[j][i] = tmp;
        }
    }
}

void matrixwhirl(struct strmatrix *p) 
{
    if (p->rownum != p->columnnum)
        return;
    struct strmatrix tmp = *p;
    int i, j;
    for (i = 0; i < p->rownum; ++i) {
        for (j = 0; j < p->columnnum; ++j) {
            p->matrix[j][p->columnnum-i-1] = tmp.matrix[i][j];
        }
    }
}

void matrixplus(struct strmatrix *plu, struct strmatrix *p, struct strmatrix *q) 
{
    if (p->columnnum != q->columnnum || p->rownum != q->rownum)
        return;
    plu->rownum = p->rownum;
    plu->columnnum = p->columnnum;
    int i, j;
    for (i = 0; i < p->rownum; ++i) {
        for (j = 0; j < p->columnnum; ++j) {
            plu->matrix[i][j] = p->matrix[i][j] + q->matrix[i][j];
        }
    }
}

void matrixmultiply(struct strmatrix *mul, struct strmatrix *p, struct strmatrix *q) 
{
    if (p->columnnum != q->rownum)
        return;
    int i, j, k;
    mul->rownum = p->rownum;
    mul->columnnum = q->columnnum;
    for (i = 0; i < mul->rownum; ++i) {
        for (j = 0; j < mul->columnnum; ++j) {
            mul->matrix[i][j] = 0;
            for (k = 0; k < p->columnnum; ++k) {
                mul->matrix[i][j] += p->matrix[i][k] * q->matrix[k][j];
            }
        }
    }
}

void TestCase01_1()
{
    int j = 0;
    int k = 0;
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  0,  0,
                                4,  5,  6,  0,  0,
                                7,  8,  9,  0,  0,
                                0,  0,  0,  0,  0,
                                0,  0,  0,  0,  0}; 
    struct strmatrix p = {0};
    p.rownum = 3;
    p.columnnum = 3;
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
        }
    }

    setnull(&p);

    assert(p.rownum == 0);
    assert(p.columnnum == 0);
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            assert(p.matrix[j][k] == 0);
        }
    }
}



void TestCase02_1()
{
    int j = 0;
    int k = 0;
    int elementnum = 0;
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  4,  5,
                                6,  7,  8,  9, 10,
                               11, 12, 13, 14, 15,
                               16, 17, 18, 19, 20,
                                0,  0,  0,  0,  0};

    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 5;
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
        }
    }

    elementnum = getelementnum(&p);

    assert(elementnum == 20);
}

void TestCase03_1()
{
    int j = 0;
    int k = 0;
    int xnum = 0;
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  4,  5,
                                6,  5,  8,  9, 10,
                               11, 12, 13, 14, 15,
                               16, 17, 18,  5, 20,
                                0,  0,  0,  0,  0};

    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 5;
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
        }
    }

    xnum = getxnum(&p, 5);

    assert(xnum == 3);
}

void TestCase04_1()
{
    int j = 0;
    int k = 0;
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  4,  0,
                                5,  6,  7,  8,  0,
                                9, 10, 11, 12,  0,
                               13, 14, 15, 16,  0,
                                0,  0,  0,  0,  0};

    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 4;
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
        }
    }

    int x[4] = {17,18,19,20};

    insertrow(&p, 2, x);

    int b[MAXSIZE][MAXSIZE] =   { 1,  2,  3,  4,  0,
                                  5,  6,  7,  8,  0,
                                 17, 18, 19, 20,  0,
                                  9, 10, 11, 12,  0,
                                 13, 14, 15, 16,  0};

    assert(p.rownum == 5);
    assert(p.columnnum == 4);
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            assert(p.matrix[j][k] == b[j][k]);
        }
    }
}

void TestCase05_1()
{
    int j = 0;
    int k = 0;
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  4,  0,
                                5,  6,  7,  8,  0,
                                9, 10, 11, 12,  0,
                               13, 14, 15, 16,  0,
                                0,  0,  0,  0,  0};

    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 4;
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
        }
    }

    int x[4] = {17,18,19,20};

    insertcolumn(&p, 2, x);

    int b[MAXSIZE][MAXSIZE] =   { 1,  2, 17, 3,  4,
                                  5,  6, 18, 7,  8,
                                  9, 10, 19, 11, 12,
                                 13, 14, 20, 15, 16,
                                  0,  0,  0,  0,  0};

    assert(p.rownum == 4);
    assert(p.columnnum == 5);
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            assert(p.matrix[j][k] == b[j][k]);
        }
    }
}

void TestCase06_1()
{
    int j = 0;
    int k = 0;
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  4,  0,
                                5,  6,  7,  8,  0,
                                9, 10, 11, 12,  0,
                               13, 14, 15, 16,  0,
                                0,  0,  0,  0,  0};

    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 4;
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
        }
    }

    matrixtrans(&p);

    int b[MAXSIZE][MAXSIZE] =   { 1,  5,  9, 13,  0,
                                  2,  6, 10, 14,  0,
                                  3,  7, 11, 15,  0,
                                  4,  8, 12, 16,  0,
                                  0,  0,  0,  0,  0};

    assert(p.rownum == 4);
    assert(p.columnnum == 4);
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            assert(p.matrix[j][k] == b[j][k]);
        }
    }
}

void TestCase07_1()
{
    int j = 0;
    int k = 0;
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  4,  0,
                                5,  6,  7,  8,  0,
                                9, 10, 11, 12,  0,
                               13, 14, 15, 16,  0,
                                0,  0,  0,  0,  0};

    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 4;
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
        }
    }

    matrixwhirl(&p);

    int b[MAXSIZE][MAXSIZE] =   { 13,  9,  5,  1,  0,
                                  14, 10,  6,  2,  0,
                                  15, 11,  7,  3,  0,
                                  16, 12,  8,  4,  0,
                                   0,  0,  0,  0,  0};

    assert(p.rownum == 4);
    assert(p.columnnum == 4);
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            assert(p.matrix[j][k] == b[j][k]);
        }
    }
}

void TestCase08_1()
{
    int j = 0;
    int k = 0;
    struct strmatrix plu = {0};
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  0,  0,
                                4,  5,  6,  0,  0,
                                7,  8,  9,  0,  0,
                               10, 11, 12,  0,  0,
                                0,  0,  0,  0,  0};
    int b[MAXSIZE][MAXSIZE] = { 1,  2,  3,  0,  0,
                                4,  5,  6,  0,  0,
                                7,  8,  9,  0,  0,
                               10, 11, 12,  0,  0,
                                0,  0,  0,  0,  0};


    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 3;

    struct strmatrix q = {0};
    q.rownum = 4;
    q.columnnum = 3;

    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
            q.matrix[j][k] = b[j][k];
        }
    }

    matrixplus(&plu, &p, &q);

    int c[MAXSIZE][MAXSIZE] =   {  2,  4,  6,  0,  0,
                                   8, 10, 12,  0,  0,
                                  14, 16, 18,  0,  0,
                                  20, 22, 24,  0,  0,
                                   0,  0,  0,  0,  0};

    assert(plu.rownum == 4);
    assert(plu.columnnum == 3);
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            assert(plu.matrix[j][k] == c[j][k]);
        }
    }
}

void TestCase09_1()
{
    int j = 0;
    int k = 0;
    struct strmatrix mul = {0};
    int a[MAXSIZE][MAXSIZE] = { 1,  2,  3,  0,  0,
                                4,  5,  6,  0,  0,
                                7,  8,  9,  0,  0,
                               10, 11, 12,  0,  0,
                                0,  0,  0,  0,  0};
    int b[MAXSIZE][MAXSIZE] = { 1,  2,  3,  4,  0,
                                5,  6,  7,  8,  0,
                                9, 10, 11, 12,  0,
                                0,  0,  0,  0,  0,
                                0,  0,  0,  0,  0};

    struct strmatrix p = {0};
    p.rownum = 4;
    p.columnnum = 3;

    struct strmatrix q = {0};
    q.rownum = 3;
    q.columnnum = 4;

    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            p.matrix[j][k] = a[j][k];
            q.matrix[j][k] = b[j][k];
        }
    }

    matrixmultiply(&mul, &p, &q);

    int c[MAXSIZE][MAXSIZE] =   {  38,  44,  50,  56,  0,
                                   83,  98, 113, 128,  0,
                                  128, 152, 176, 200,  0,
                                  173, 206, 239, 272,  0,
                                    0,   0,   0,   0,  0};

    assert(mul.rownum == 4);
    assert(mul.columnnum == 4);
    for(j = 0; j < MAXSIZE; j++)
    {
        for(k = 0; k < MAXSIZE; k++)
        {
            assert(mul.matrix[j][k] == c[j][k]);
        }
    }
}

int main(void)
{
    TestCase01_1();
    TestCase02_1();
    TestCase03_1();
    TestCase04_1();
    TestCase05_1();
    TestCase06_1();
    TestCase07_1();
    TestCase08_1();
    TestCase09_1();
    return 0;
}
