#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>

bool isValidIPPart(int cur, int len)
{
    if (len > 3 || len == 0)
        return false;
    if (len == 3)
        return cur > 99 && cur < 256;
    if (len == 2)
        return cur > 9 && cur < 100;
    return cur < 10;
}

bool isIPAddressValid(const char* pszIPAddr)
{
    if (!pszIPAddr)
        return false;
    while (isspace(*pszIPAddr))
        ++pszIPAddr;
    const char *end = pszIPAddr;
    while (*end)
        ++end;
    while (isspace(*(end-1)))
        --end;
    int npart = 0, cur = 0;
    const char *prev = pszIPAddr;
    for (; pszIPAddr != end; ++pszIPAddr) {
        if (isdigit(*pszIPAddr)) {
            cur = cur * 10 + *pszIPAddr - '0';
        } else if (*pszIPAddr == '.') {
            if (!isValidIPPart(cur, pszIPAddr-prev))
                return false;
            cur = 0;
            prev = pszIPAddr+1;
            ++npart;
        } else
            return false;
    }
    return npart == 3 && isValidIPPart(cur, pszIPAddr-prev);
}

int main(void)
{
    printf("%d\n", isIPAddressValid("   0.0.0.0   "));
    printf("%d\n", isIPAddressValid("0.0.0. 0"));
    printf("%d\n", isIPAddressValid("0.0.0.0"));
    printf("%d\n", isIPAddressValid("01.0.0.0"));
    printf("%d\n", isIPAddressValid("1.02.3.4"));
    printf("%d\n", isIPAddressValid("0.0.0"));
    printf("%d\n", isIPAddressValid("0.0.0.255"));
    printf("%d\n", isIPAddressValid("0.0.0.256"));
    return 0;
}
