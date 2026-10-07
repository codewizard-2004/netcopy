#include <stdio.h>
#include <string.h>

#define MAX 100

char stack[MAX];
char input[MAX];

int top = -1;
int ip = 0;

/* Push a symbol onto the stack */
void shift()
{
    stack[++top] = input[ip++];
}

/* Reduce according to the grammar */
int reduce()
{
    /*
       T -> i
    */
    if (top >= 0 && stack[top] == 'i')
    {
        stack[top] = 'T';

        printf("REDUCE T -> i\n");
        return 1;
    }

    /*
       E -> E + T
    */
    if (top >= 2 &&
        stack[top - 2] == 'E' &&
        stack[top - 1] == '+' &&
        stack[top] == 'T')
    {
        top -= 2;
        stack[top] = 'E';

        printf("REDUCE E -> E+T\n");
        return 1;
    }

    /*
       E -> T
    */
    if (top >= 0 && stack[top] == 'T')
    {
        stack[top] = 'E';

        printf("REDUCE E -> T\n");
        return 1;
    }

    return 0;
}

/* Print stack */
void printStack()
{
    int i;

    for (i = 0; i <= top; i++)
        printf("%c", stack[i]);
}

/* Print remaining input */
void printInput()
{
    int i;

    for (i = ip; input[i] != '\0'; i++)
        printf("%c", input[i]);
}

int main()
{
    printf("Enter the input string: ");
    scanf("%s", input);

    printf("\n%-15s %-15s Action\n",
           "Stack", "Input");

    printf("----------------------------------------\n");

    while (1)
    {
        /*
           Try reduction first
        */
        if (reduce())
        {
            printf("%-15s %-15s\n", "", "");
            printStack();
            printf("             ");
            printInput();
            printf("\n");
        }

        /*
           If input remains, shift
        */
        else if (input[ip] != '\0')
        {
            shift();

            printf("%-15s %-15s SHIFT\n", "", "");

            printStack();
            printf("             ");
            printInput();
            printf("\n");
        }

        /*
           If input is finished and stack contains E
        */
        else if (top == 0 && stack[top] == 'E')
        {
            printf("\nACCEPT\n");
            break;
        }

        /*
           Otherwise error
        */
        else
        {
            printf("\nERROR\n");
            break;
        }
    }

    return 0;
}