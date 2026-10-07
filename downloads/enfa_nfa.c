#include <stdio.h>

#define MAX_STATES 20
#define MAX_SYMBOLS 10

int n;                          // Number of states
int m;                          // Number of input symbols

char symbols[MAX_SYMBOLS];      // Input symbols

int epsilon[MAX_STATES][MAX_STATES];

// trans[s][i][j] = 1 if state i --symbols[s]--> state j
int trans[MAX_SYMBOLS][MAX_STATES][MAX_STATES];

int finalState[MAX_STATES];
int newFinalState[MAX_STATES];

/* --------------------------------------------------
   Find epsilon closure of a state
   closure[i] = 1 means qi belongs to the closure
   -------------------------------------------------- */
void epsilonClosure(int state, int closure[])
{
    int stack[MAX_STATES];
    int top = -1;

    for (int i = 0; i < n; i++)
        closure[i] = 0;

    closure[state] = 1;
    stack[++top] = state;

    while (top >= 0)
    {
        int current = stack[top--];

        for (int next = 0; next < n; next++)
        {
            if (epsilon[current][next] && !closure[next])
            {
                closure[next] = 1;
                stack[++top] = next;
            }
        }
    }
}

/* --------------------------------------------------
   Print a set of states
   -------------------------------------------------- */
void printSet(int states[])
{
    printf("{ ");

    for (int i = 0; i < n; i++)
    {
        if (states[i])
            printf("q%d ", i);
    }

    printf("}");
}

int main()
{
    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter number of input symbols: ");
    scanf("%d", &m);

    printf("Enter the input symbols:\n");

    for (int i = 0; i < m; i++)
        scanf(" %c", &symbols[i]);

    /* Read epsilon transition matrix */
    printf("\nEnter epsilon transition matrix:\n");
    printf("1 = epsilon transition exists, 0 = no transition\n");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &epsilon[i][j]);
        }
    }

    /* Read transitions for each symbol */
    for (int s = 0; s < m; s++)
    {
        printf("\nEnter transition matrix for symbol '%c':\n",
               symbols[s]);

        for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                scanf("%d", &trans[s][i][j]);
            }
        }
    }

    /* Read final states */
    int numberOfFinalStates;

    printf("\nEnter number of final states: ");
    scanf("%d", &numberOfFinalStates);

    printf("Enter final states: ");

    for (int i = 0; i < numberOfFinalStates; i++)
    {
        int state;
        scanf("%d", &state);
        finalState[state] = 1;
    }

    /* --------------------------------------------------
       Find new final states

       q is final in new NFA if epsilon-closure(q)
       contains an original final state.
       -------------------------------------------------- */

    for (int q = 0; q < n; q++)
    {
        int closure[MAX_STATES];

        epsilonClosure(q, closure);

        for (int i = 0; i < n; i++)
        {
            if (closure[i] && finalState[i])
            {
                newFinalState[q] = 1;
                break;
            }
        }
    }

    /* --------------------------------------------------
       Construct NFA without epsilon transitions
       -------------------------------------------------- */

    printf("\n============================================\n");
    printf("NFA WITHOUT EPSILON TRANSITIONS\n");
    printf("============================================\n");

    for (int q = 0; q < n; q++)
    {
        int closure[MAX_STATES];

        epsilonClosure(q, closure);

        printf("\nState q%d:\n", q);

        for (int s = 0; s < m; s++)
        {
            int result[MAX_STATES] = {0};

            /*
             * Step 1:
             * For every state in epsilon-closure(q),
             * follow symbol 's'
             */
            for (int p = 0; p < n; p++)
            {
                if (closure[p])
                {
                    for (int r = 0; r < n; r++)
                    {
                        if (trans[s][p][r])
                        {
                            result[r] = 1;
                        }
                    }
                }
            }

            /*
             * Step 2:
             * Take epsilon-closure of every resulting state
             */
            int finalResult[MAX_STATES] = {0};

            for (int r = 0; r < n; r++)
            {
                if (result[r])
                {
                    int temp[MAX_STATES];

                    epsilonClosure(r, temp);

                    for (int k = 0; k < n; k++)
                    {
                        if (temp[k])
                            finalResult[k] = 1;
                    }
                }
            }

            printf("  on '%c' -> ", symbols[s]);
            printSet(finalResult);
            printf("\n");
        }
    }

    /* Print final states */
    printf("\nNew final states: { ");

    for (int i = 0; i < n; i++)
    {
        if (newFinalState[i])
            printf("q%d ", i);
    }

    printf("}\n");

    return 0;
}

/*
Enter number of states: 3
Enter number of input symbols: 2
Enter the input symbols:
a b

Enter epsilon transition matrix:
1 = epsilon transition exists, 0 = no transition
0 1 0
0 0 0
0 0 0

Enter transition matrix for symbol 'a':
0 0 0
0 1 0
0 0 0

Enter transition matrix for symbol 'b':
0 0 0
0 0 1
0 0 0

Enter number of final states: 1
Enter final states: 2
*/