#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "4425_slice_iterator.h"

int check_slice_desc(const SliceDesc *slice_desc)
{
    if (!slice_desc || !slice_desc->first_element_of_orignal_array || slice_desc->coord_num > MAX_COORD_NUM || slice_desc->coord_num < 1)
        return 0;
    int i;
    for (i = 0; i < slice_desc->coord_num; ++i) {
        if (slice_desc->coord_slice[i].start > slice_desc->coord_slice[i].end || slice_desc->coord_slice[i].end > slice_desc->coord_spec[i]
                || slice_desc->coord_slice[i].start < 0)
            return 0;
    }
    return 1;
}

int SliceIteratorInit (const SliceDesc* slice_desc, SliceIterator* slice_iterator)
{
    if (!check_slice_desc(slice_desc))
        return -1;
    slice_iterator->slice_desc = *slice_desc;
    unsigned int i;
    for (i = 0; i < slice_desc->coord_num; ++i) {
        slice_iterator->coord_cur[i] = slice_desc->coord_slice[i].start;
    }
    slice_iterator->isEnd = 0;
    return 0;
}

int* SliceIteratorGetCurr(const SliceIterator* slice_iterator)
{
    if (SliceIteratorEnd(slice_iterator))
        return NULL;
    unsigned int offset = 0, i;
    for (i = 0; i < slice_iterator->slice_desc.coord_num; ++i) {
        offset = offset * (slice_iterator->slice_desc.coord_spec[i]+1) + slice_iterator->coord_cur[i];
    }
    return slice_iterator->slice_desc.first_element_of_orignal_array + offset;
}

void SliceIteratorNext(SliceIterator* slice_iterator)
{
    unsigned int i;
    for (i = slice_iterator->slice_desc.coord_num; i > 0; --i) {
        if (slice_iterator->slice_desc.coord_slice[i-1].end == slice_iterator->coord_cur[i-1])
            slice_iterator->coord_cur[i-1] = slice_iterator->slice_desc.coord_slice[i-1].start;
        else {
            slice_iterator->coord_cur[i-1] += 1;
            return;
        }
    }
    slice_iterator->isEnd = 1;
}

int SliceIteratorEnd(const SliceIterator* slice_iterator)
{
    return slice_iterator->isEnd;
}

int CopySlice (const SliceDesc* slice_desc,int** first_element_of_slice_array,unsigned long* slice_element_num)
{
    SliceIterator si, *slice_iterator = &si;
    if (-1 == SliceIteratorInit(slice_desc, slice_iterator))
        return -1;
    unsigned int num = 1, i;
    for (i = slice_desc->coord_num; i > 0; --i) {
        num *= slice_desc->coord_slice[i-1].end - slice_desc->coord_slice[i-1].start + 1;
    }
    *first_element_of_slice_array = (int *)malloc(sizeof(int)* num);
    *slice_element_num = num;
    int *p = *first_element_of_slice_array;
    while (!SliceIteratorEnd(slice_iterator)) {
        *p++ = *SliceIteratorGetCurr(slice_iterator);
        SliceIteratorNext(slice_iterator);
    }
    assert(p == *first_element_of_slice_array + num);
    return 0;
}

void test_case1(void)
{
    int A[10][10][3];
    int total = 0;
    int new_total = 0;

    int i, j, k;
    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            for(k=0;k<3;k++)
    {
        A[i][j][k] = i*j*k;
        total += A[i][j][k];
    }

    SliceIterator slice_iterator;
    SliceDesc slice_desc = {&A[0][0][0],3,{9,9,2},{{2,4},{0,6},{1,2}}};

    for(SliceIteratorInit(&slice_desc,&slice_iterator);1!=SliceIteratorEnd(&slice_iterator);SliceIteratorNext(&slice_iterator) )
    {
        *SliceIteratorGetCurr(&slice_iterator) += 1;
    }


    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            for(k=0;k<3;k++)
    {
        new_total += A[i][j][k];
    }

    assert(new_total > total);
}

void test_case2(void)
{
    int A[10][10][3];
    int i, j, k;
    for(i=0;i<10;i++)
        for(j=0;j<10;j++)
            for(k=0;k<3;k++)
    {
        A[i][j][k] = 1;
    }

    SliceDesc slice_desc = {&A[0][0][0],3,{9,9,2},{{2,4},{0,6},{1,2}}};
    int* first_element_of_slice_array = NULL;
    unsigned long slice_element_num = 0;

    int ret = CopySlice(&slice_desc,&first_element_of_slice_array,&slice_element_num);

    assert(0 == ret);
    for(i=0;i<slice_element_num; i++)
    {
        assert(1 == first_element_of_slice_array[i]);
    }
    free(first_element_of_slice_array);
}

void test_case3(void)
{
    int A[10][8];
    int i, j, k;
    for(i=0;i<10;i++)
        for(j=0;j<8;j++)
    {
        A[i][j] = 1;
    }

    SliceDesc slice_desc = {&A[0][0],2,{9,7},{{0,0},{0,0}}};
    int* first_element_of_slice_array = NULL;
    unsigned long slice_element_num = 0;

    int ret = CopySlice(&slice_desc,&first_element_of_slice_array,&slice_element_num);

    assert(0 == ret && slice_element_num == 1);
    for(i=0;i<slice_element_num; i++)
    {
        assert(1 == first_element_of_slice_array[i]);
    }
    free(first_element_of_slice_array);
}

int main(void)
{
    test_case1();
    test_case2();
    test_case3();
    return 0;
}
