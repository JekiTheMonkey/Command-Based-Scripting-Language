#include "string_helper.h"


int stricmp(const char *p_str_lhs, const char *p_str_rhs)
{
    while (
        (*p_str_lhs      == *p_str_rhs
    ||   *p_str_lhs + 32 == *p_str_rhs
    ||   *p_str_lhs - 32 == *p_str_rhs
        )
    &&  *p_str_lhs != '\0'
    &&  *p_str_rhs != '\0')
    {
        ++p_str_lhs;
        ++p_str_rhs;
    }

    // Equal 0, lhs > rhs 1, lhs < rhs -1
    return *p_str_lhs == *p_str_rhs ? 0 : 
           *p_str_lhs > *p_str_rhs ? 1 : -1;
}

char *strupr(char *p_str)
{
    char *p_tmp = p_str;
    char ch;
    while ((ch = *p_str) != '\0')
    {
        *p_str -= (ch >= 97 && ch <= 122) ? 32 : 0;
        ++p_str;
    }

    return p_tmp;
}

int is_present(const char *p_str, char ch)
{
    char tmp;

    while ((tmp = *p_str) != '\0')
    {
        if (tmp == ch)
            return 1;
        ++p_str;
    }

    return 0;
}
