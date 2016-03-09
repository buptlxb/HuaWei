#include <stdio.h>
#include <stdlib.h>

int IsConflict(int board[], int n, int row, int col)
{
    int i;
    for (i = 0; i < row; ++i) {
        if (board[i] == col || row - i == col - board[i] || row - i == board[i] - col)
            return 1;
    }
    return 0;
}

void PlaceQueenMethodNumIter(int board[], int n, int row, int *num)
{
    if (row == n) {
        *num += 1;
        return;
    }
    int i;
    for (i = 0; i < n; ++i) {
        if (IsConflict(board, n, row, i))
            continue;
        board[row] = i;
        PlaceQueenMethodNumIter(board, n, row+1, num);
    }
}

int PlaceQueenMethodNum(int n)
{
    int num = 0;
    if (n <= 0)
        return num;
    int board[n];
    PlaceQueenMethodNumIter(board, n, 0, &num);
    return num;
}

int main(void)
{
    int i;
    for (i = 0; i < 10; ++i)
        printf("%d -> %d\n", i, PlaceQueenMethodNum(i));
    return 0;
}
