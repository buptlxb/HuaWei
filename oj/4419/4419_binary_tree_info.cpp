#include "4419_binary_tree_info.hh"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

unsigned int getHeight(BiNode *head)
{
    if (!head)
        return 0;
    unsigned int a = getHeight(head->left);
    unsigned int b = getHeight(head->right);
    return (a > b ? a : b) + 1;
}

void getWidths(BiNode *head, unsigned int *pulWidth, unsigned int h)
{
    if (!head)
        return;
    ++pulWidth[h];
    getWidths(head->left, pulWidth, h+1);
    getWidths(head->right, pulWidth, h+1);
}

int GetBiNodeInfo(BiNode &head, unsigned int *pulWidth, unsigned int *pulHeight)
{
    if (!pulWidth || !pulWidth)
        return 1;
    *pulHeight = getHeight(&head);
    unsigned int *widths = (unsigned int *)calloc(*pulHeight, sizeof(unsigned int));
    getWidths(&head, widths, 0);
    *pulWidth = 0;
    unsigned int i;
    for (i = 0; i < *pulHeight; ++i) {
        if (*pulWidth < widths[i])
            *pulWidth = widths[i];
    }
    free(widths);
    return 0;
}

int main(void)
{
    BiNode d = {'d', 0, 0};
    BiNode e = {'e', 0, 0};
    BiNode f = {'f', 0, 0};
    BiNode g = {'g', 0, 0};
    BiNode b = {'b', &d, &e};
    BiNode c = {'c', &f, &g};
    BiNode a = {'a', &b, &c};
    unsigned int w, h;
    int ret = GetBiNodeInfo(a, &w, &h);
    assert(ret == 0 && w == 4 && h == 3);
    return 0;
}
