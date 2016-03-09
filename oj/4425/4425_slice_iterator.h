#ifndef SLICE_ITERATOR_H
#define SLICE_ITERATOR_H

#define MAX_COORD_NUM 10

typedef struct
{
    unsigned long start;
    unsigned long end; 
}CoordSlice;

typedef struct
{
    int* first_element_of_orignal_array;
    unsigned long coord_num; 
    unsigned long coord_spec[MAX_COORD_NUM];
    CoordSlice coord_slice[MAX_COORD_NUM];
}SliceDesc;

typedef struct
{
    SliceDesc slice_desc;
    unsigned long coord_cur[MAX_COORD_NUM];
    int isEnd;
}SliceIterator;

int SliceIteratorInit (const SliceDesc* slice_desc, SliceIterator* slice_iterator);
int* SliceIteratorGetCurr(const SliceIterator* slice_iterator);
void SliceIteratorNext(SliceIterator* slice_iterator);
int SliceIteratorEnd(const SliceIterator* slice_iterator);
int CopySlice (const SliceDesc* slice_desc,int** first_element_of_slice_array,unsigned long* slice_element_num);

#endif

