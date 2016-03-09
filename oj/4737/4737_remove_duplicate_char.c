#include <stdio.h>

int GetResult(const char *input, char *output)
{
    if (!input || !output)
        return -1;
    char map[256] = {0, };
    int i, j;
    for (i = j = 0; input[i]; ++i) {
        if (map[input[i]])
            continue;
        output[j++] = input[i];
        map[input[i]] = 1;
    }
    output[j] = input[i];
    return 0;
}

int main(void)
{
    char buf[100];
    printf("%d\n", GetResult("abadcbad", buf));
    puts(buf);
    puts(buf);
    return 0;
}
