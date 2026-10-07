#include <stdio.h>
#include <string.h>

char input[100];
int pos = 0;

/* Function declarations */
int E();
int T();

/* E -> T + E | T */
int E()
{
    /* First parse T */
    if (!T())
        return 0;

    /* If '+' is present, parse another E */
    if (input[pos] == '+')
    {
        pos++;

        if (!E())
            return 0;
    }

    return 1;
}

/* T -> i */
int T()
{
    if (input[pos] == 'i')
    {
        pos++;
        return 1;
    }

    return 0;
}

int main()
{
    printf("Enter the expression: ");
    scanf("%s", input);

    /* Parse starting from E */
    if (E() && input[pos] == '\0')
        printf("Valid expression\n");
    else
        printf("Invalid expression\n");

    return 0;
}