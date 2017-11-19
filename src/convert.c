/*
 * convert.c
 *
 *  Created on: 31.10.2017
 *      Author: a copy from Stack Exchange
 */


/*
 * Convert a hexadecimal string into an integer value
 *
 */

int htoi(const char *s, unsigned int *res)
{
        if ('0' == s[0] && ('x' == s[1] || 'X' == s[1]))
        {
                s += 2;
        }
        if ('\0' == *s)
        {
                return -1;
        }
        unsigned int rc;
        int n;

        for (rc = 0; '\0' != (n = *s); s++)
        {
                if ( n >= 'a' && n <= 'f')
                {
                        n = n - 'a' + 10;
                }
                else if (n >= 'A' && n <= 'F')
                {
                        n = n - 'A' + 10;
                }
                else if (n >= '0' && n <= '9')
                {
                        n = n - '0';
                }
                else
                {
                        return -1;
                }

                rc *= 16;
                rc +=  n;
        }

        *res = rc;
        return 0;
}

/*
 * Convert a hexadecimal string into an integer value
 *
 */

int atoi(const char* s)

{
    unsigned int a;
    htoi(s,&a);
    return (a);
}

