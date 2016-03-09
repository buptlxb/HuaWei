#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define OFF 0
#define ON 1

unsigned int Continumax(char** pOutputstr,  const char* intputstr)
{
    int max = 0, cur = 0, state = OFF;
    const char *res = NULL;
    const char *start = intputstr;
    while (*intputstr) {
        if (isdigit(*intputstr)) {
            if (state)
                ++cur;
            else {
                state = ON;
                start = intputstr;
                cur = 1;
            }
        } else {
            if (state) {
                if (cur >= max) {
                    max = cur;
                    res = start;
                }
                state = OFF;
                cur = 0;
            }
        }
        ++intputstr;
    }
    if (cur >= max) {
        max = cur;
        res = start;
    }
    *pOutputstr = (char *)malloc(max+1);
    (*pOutputstr)[max] = '\0';
    if (!max)
        return 0;
    strncpy(*pOutputstr, res, max);
    return max;
}

int main(void)
{
    char *ret;
    int num;
    num = Continumax(&ret, "abcd12345ed125ss123058789");
    printf("%s : %d\n", ret, num);
    free(ret);
    num = Continumax(&ret, "abcd12345ss54761");
    printf("%s : %d\n", ret, num);
    free(ret);
    return 0;
}
