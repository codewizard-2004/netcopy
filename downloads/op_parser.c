#include <stdio.h>
#include <string.h>

#define MAX 100

char stack[MAX];
char input[MAX];
int top = -1;
int ip = 0;

/* Operator precedence table
 *
 *           +     -    *    /    ^    (    )    i    $
 */
char precedence[9][9] = {
    /* + */ {'>', '>', '<', '<', '<', '<', '>', '<', '>'},
    /* - */ {'>', '>', '<', '<', '<', '<', '>', '<', '>'},
    /* * */ {'>', '>', '>', '>', '<', '<', '>', '<', '>'},
    /* / */ {'>', '>', '>', '>', '<', '<', '>', '<', '>'},
    /* ^ */ {'>', '>', '>', '>', '<', '<', '>', '<', '>'},
    /* ( */ {'<', '<', '<', '<', '<', '<', '=', '<', ' '},
    /* ) */ {'>', '>', '>', '>', '>', ' ', '>', ' ', '>'},
    /* i */ {'>', '>', '>', '>', '>', ' ', '>', ' ', '>'},
    /* $ */ {'<', '<', '<', '<', '<', '<', ' ', '<', '='}
};

/* Get index of terminal */
int getIndex(char c)
{
    switch (c)
    {
        case '+': return 0;
        case '-': return 1;
        case '*': return 2;
        case '/': return 3;
        case '^': return 4;
        case '(': return 5;
        case ')': return 6;
        case 'i': return 7;
        case '$': return 8;
        default: return -1;
    }
}

/* Find the top-most terminal in stack */
int topTerminal()
{
    int i;

    for (i = top; i >= 0; i--)
    {
        if (getIndex(stack[i]) != -1)
            return i;
    }

    return -1;
}

/* Print current stack */
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

/* Reduce handle to E */
int reduce()
{
    /*
       Valid handles:
       i
       E+E
       E-E
       E*E
       E/E
       E^E
       (E)
    */

    if (top >= 0 && stack[top] == 'i')
    {
        stack[top] = 'E';
        return 1;
    }

    if (top >= 2 &&
        stack[top] == 'E' &&
        stack[top - 1] == 'E')
    {
        /*
           This case is invalid because
           two E's cannot directly form a handle.
        */
        return 0;
    }

    if (top >= 2 &&
        stack[top] == 'E' &&
        stack[top - 2] == 'E')
    {
        char op = stack[top - 1];

        if (op == '+' || op == '-' ||
            op == '*' || op == '/' || op == '^')
        {
            top -= 2;
            stack[top] = 'E';
            return 1;
        }
    }

    if (top >= 2 &&
        stack[top] == ')' &&
        stack[top - 1] == 'E' &&
        stack[top - 2] == '(')
    {
        top -= 2;
        stack[top] = 'E';
        return 1;
    }

    return 0;
}

int main()
{
    char relation;
    int sIndex, iIndex;

    printf("Enter expression using i for identifier: ");
    scanf("%s", input);

    /* Add end marker */
    int len = strlen(input);
    input[len] = '$';
    input[len + 1] = '\0';

    /* Initialize stack */
    top = 0;
    stack[top] = '$';

    printf("\n%-20s %-20s Action\n", "Stack", "Input");
    printf("------------------------------------------------------------\n");

    while (1)
    {
        /* Accepted */
        if (stack[top] == '$' &&
            input[ip] == '$' &&
            top == 1 &&
            stack[1] == 'E')
        {
            printf("%-20s %-20s ACCEPT\n", "$E", "$");
            break;
        }

        /* Get top terminal */
        int t = topTerminal();

        if (t == -1)
        {
            printf("ERROR\n");
            break;
        }

        sIndex = getIndex(stack[t]);
        iIndex = getIndex(input[ip]);

        if (iIndex == -1)
        {
            printf("ERROR: Invalid symbol '%c'\n", input[ip]);
            break;
        }

        relation = precedence[sIndex][iIndex];

        /* SHIFT */
        if (relation == '<' || relation == '=')
        {
            printf("%-20s %-20s SHIFT %c\n",
                   "", "", input[ip]);

            stack[++top] = input[ip];
            ip++;
        }

        /* REDUCE */
        else if (relation == '>')
        {
            if (reduce())
            {
                printf("%-20s %-20s REDUCE\n", "", "");
            }
            else
            {
                printf("%-20s %-20s ERROR\n", "", "");
                break;
            }
        }

        else
        {
            printf("%-20s %-20s ERROR\n", "", "");
            break;
        }

        /* Display actual stack/input after action */
        printf("     ");
        printStack();
        printf("            ");
        printInput();
        printf("\n");
    }

    return 0;
}