#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 20

char production[MAX][MAX];
char first[MAX][MAX];
char follow[MAX][MAX];

int n;

/* Check whether a character is already present in a set */
int contains(char set[], char c)
{
    int i;

    for (i = 0; set[i] != '\0'; i++)
    {
        if (set[i] == c)
            return 1;
    }

    return 0;
}

/* Add a character to a set */
void addToSet(char set[], char c)
{
    int len;

    if (!contains(set, c))
    {
        len = strlen(set);
        set[len] = c;
        set[len + 1] = '\0';
    }
}

/* Find FIRST of a symbol */
void findFirst(char symbol, int pos)
{
    int i, j;
    char next;

    /* Terminal */
    if (!isupper(symbol))
    {
        addToSet(first[pos], symbol);
        return;
    }

    /* Non-terminal */
    for (i = 0; i < n; i++)
    {
        if (production[i][0] == symbol)
        {
            /* If production is A -> # */
            if (production[i][3] == '#')
            {
                addToSet(first[pos], '#');
                continue;
            }

            for (j = 3; production[i][j] != '\0'; j++)
            {
                next = production[i][j];

                /* Terminal */
                if (!isupper(next))
                {
                    addToSet(first[pos], next);
                    break;
                }

                /* Non-terminal */
                findFirst(next, pos);

                /* If epsilon is not present, stop */
                if (!contains(first[pos], '#'))
                    break;
            }
        }
    }
}

/* Find FOLLOW of a non-terminal */
void findFollow(char symbol, int pos)
{
    int i, j, k;
    char next;

    /* Start symbol gets $ */
    if (pos == 0)
        addToSet(follow[pos], '$');

    for (i = 0; i < n; i++)
    {
        for (j = 3; production[i][j] != '\0'; j++)
        {
            if (production[i][j] == symbol)
            {
                /* Symbol is at the end */
                if (production[i][j + 1] == '\0')
                {
                    char lhs = production[i][0];

                    if (lhs != symbol)
                    {
                        for (k = 0; k < n; k++)
                        {
                            if (production[k][0] == lhs)
                            {
                                findFollow(lhs, k);

                                int x;
                                for (x = 0; follow[k][x] != '\0'; x++)
                                {
                                    addToSet(follow[pos], follow[k][x]);
                                }
                                break;
                            }
                        }
                    }
                }
                else
                {
                    next = production[i][j + 1];

                    /* Next symbol is terminal */
                    if (!isupper(next))
                    {
                        addToSet(follow[pos], next);
                    }
                    else
                    {
                        /*
                         * Add FIRST(next) - {#}
                         */
                        for (k = 0; k < n; k++)
                        {
                            if (production[k][0] == next)
                            {
                                findFirst(next, k);

                                int x;
                                for (x = 0; first[k][x] != '\0'; x++)
                                {
                                    if (first[k][x] != '#')
                                        addToSet(follow[pos], first[k][x]);
                                }

                                /*
                                 * If next can produce epsilon,
                                 * add FOLLOW of LHS
                                 */
                                if (contains(first[k], '#'))
                                {
                                    char lhs = production[i][0];

                                    if (lhs != symbol)
                                    {
                                        int y;

                                        for (y = 0; y < n; y++)
                                        {
                                            if (production[y][0] == lhs)
                                            {
                                                findFollow(lhs, y);

                                                int z;
                                                for (z = 0;
                                                     follow[y][z] != '\0';
                                                     z++)
                                                {
                                                    addToSet(
                                                        follow[pos],
                                                        follow[y][z]);
                                                }
                                                break;
                                            }
                                        }
                                    }
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

int main()
{
    int i, j;
    char nonTerminals[MAX];

    printf("Enter number of productions: ");
    scanf("%d", &n);

    printf("\nEnter productions in the form A->abc\n");
    printf("Use # for epsilon.\n\n");

    for (i = 0; i < n; i++)
    {
        scanf("%s", production[i]);
    }

    /*
     * Initialize FIRST and FOLLOW arrays
     */
    for (i = 0; i < MAX; i++)
    {
        first[i][0] = '\0';
        follow[i][0] = '\0';
    }

    /*
     * Store unique non-terminals
     */
    int ntCount = 0;

    for (i = 0; i < n; i++)
    {
        char lhs = production[i][0];

        if (!contains(nonTerminals, lhs))
        {
            nonTerminals[ntCount++] = lhs;
            nonTerminals[ntCount] = '\0';
        }
    }

    /*
     * Find FIRST
     */
    for (i = 0; i < ntCount; i++)
    {
        char nt = nonTerminals[i];

        for (j = 0; j < n; j++)
        {
            if (production[j][0] == nt)
            {
                findFirst(nt, j);
            }
        }
    }

    /*
     * Find FOLLOW
     */
    for (i = 0; i < ntCount; i++)
    {
        char nt = nonTerminals[i];

        for (j = 0; j < n; j++)
        {
            if (production[j][0] == nt)
            {
                findFollow(nt, j);
            }
        }
    }

    /*
     * Display FIRST
     */
    printf("\nFIRST sets:\n");

    for (i = 0; i < ntCount; i++)
    {
        printf("FIRST(%c) = { ", nonTerminals[i]);

        for (j = 0; first[i][j] != '\0'; j++)
        {
            printf("%c ", first[i][j]);
        }

        printf("}\n");
    }

    /*
     * Display FOLLOW
     */
    printf("\nFOLLOW sets:\n");

    for (i = 0; i < ntCount; i++)
    {
        printf("FOLLOW(%c) = { ", nonTerminals[i]);

        for (j = 0; follow[i][j] != '\0'; j++)
        {
            printf("%c ", follow[i][j]);
        }

        printf("}\n");
    }

    return 0;
}