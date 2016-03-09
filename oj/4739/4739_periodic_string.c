#include <stdio.h>
#include <string.h>

int isPeriod(const char *inputstring, int period)
{
    int i;
    for (i = 0; inputstring[i+period]; i += period) {
        if (strncmp(inputstring, inputstring+i, period))
            return 0;
    }
    return 1;
}
int GetMinPeriod(char *inputstring)
{
    if (!inputstring)
        return 0;
    int len = strlen(inputstring);
    int i;
    for (i = 1; i <= len; ++i) {
        if (len % i)
            continue;
        if (isPeriod(inputstring, i))
            return i;
    }
    return len;
}

int main(void)
{
    printf("%s : %d\n", "abcabcabcabc", GetMinPeriod("abcabcabcabc"));
    return 0;
}
