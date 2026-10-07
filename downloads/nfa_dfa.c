#include <stdio.h>

#define MAX_STATES 20
#define MAX_SYMBOLS 10
#define MAX_DFA_STATES 1024

int n, m;

char symbols[MAX_SYMBOLS];

/*
   NFA transition table

   trans[s][i][j] = 1 means:
   state qi --symbols[s]--> qj
*/
int trans[MAX_SYMBOLS][MAX_STATES][MAX_STATES];

/* DFA states are sets of NFA states */
int dfaStates[MAX_DFA_STATES][MAX_STATES];

int dfaCount = 0;

/* Original NFA final states */
int finalState[MAX_STATES];

/* Check whether two sets of states are equal */
int sameSet(int a[], int b[])
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != b[i])
            return 0;
    }

    return 1;
}

/* Find whether a DFA state already exists */
int findState(int state[])
{
    for (int i = 0; i < dfaCount; i++)
    {
        if (sameSet(dfaStates[i], state))
            return i;
    }

    return -1;
}

/* Add a new DFA state */
int addState(int state[])
{
    int existing = findState(state);

    if (existing != -1)
        return existing;

    for (int i = 0; i < n; i++)
        dfaStates[dfaCount][i] = state[i];

    return dfaCount++;
}

/* Print a set of NFA states */
void printSet(int state[])
{
    printf("{");

    int first = 1;

    for (int i = 0; i < n; i++)
    {
        if (state[i])
        {
            if (!first)
                printf(",");

            printf("q%d", i);
            first = 0;
        }
    }

    printf("}");
}

/*
   Find all states reachable from a DFA state
   using a particular input symbol.
*/
void move(int current[], int symbolIndex, int result[])
{
    /* Initially no states */
    for (int i = 0; i < n; i++)
        result[i] = 0;

    /*
       For every NFA state in the current DFA state,
       follow the given symbol.
    */
    for (int i = 0; i < n; i++)
    {
        if (current[i])
        {
            for (int j = 0; j < n; j++)
            {
                if (trans[symbolIndex][i][j])
                {
                    result[j] = 1;
                }
            }
        }
    }
}

int main()
{
    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter number of input symbols: ");
    scanf("%d", &m);

    printf("Enter input symbols:\n");

    for (int i = 0; i < m; i++)
        scanf(" %c", &symbols[i]);

    /*
       Read NFA transition matrices
    */
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

    /*
       Read final states
    */
    int numberOfFinalStates;

    printf("\nEnter number of final states: ");
    scanf("%d", &numberOfFinalStates);

    printf("Enter final states:\n");

    for (int i = 0; i < numberOfFinalStates; i++)
    {
        int state;

        scanf("%d", &state);

        finalState[state] = 1;
    }

    /*
       -----------------------------------------
       SUBSET CONSTRUCTION
       -----------------------------------------
    */

    /*
       Initial DFA state = {q0}
    */
    int start[MAX_STATES] = {0};

    start[0] = 1;

    addState(start);

    /*
       Process every DFA state
    */
    for (int current = 0; current < dfaCount; current++)
    {
        for (int s = 0; s < m; s++)
        {
            int result[MAX_STATES];

            move(dfaStates[current], s, result);

            /*
               Add resulting set as a new DFA state
               if it doesn't already exist.
            */
            addState(result);
        }
    }

    /*
       -----------------------------------------
       PRINT DFA
       -----------------------------------------
    */

    printf("\n============================================\n");
    printf("DFA\n");
    printf("============================================\n");

    printf("\nDFA States:\n");

    for (int i = 0; i < dfaCount; i++)
    {
        printf("D%d = ", i);
        printSet(dfaStates[i]);
        printf("\n");
    }

    printf("\nDFA Transition Table:\n");

    for (int i = 0; i < dfaCount; i++)
    {
        printf("\nD%d = ", i);
        printSet(dfaStates[i]);
        printf("\n");

        for (int s = 0; s < m; s++)
        {
            int result[MAX_STATES];

            move(dfaStates[i], s, result);

            int destination = findState(result);

            printf("  on '%c' -> D%d = ",
                   symbols[s],
                   destination);

            printSet(result);

            printf("\n");
        }
    }

    /*
       -----------------------------------------
       FIND DFA FINAL STATES
       -----------------------------------------
    */

    printf("\nDFA Final States: ");

    for (int i = 0; i < dfaCount; i++)
    {
        int isFinal = 0;

        /*
           A DFA state is final if it contains
           at least one NFA final state.
        */
        for (int j = 0; j < n; j++)
        {
            if (dfaStates[i][j] && finalState[j])
            {
                isFinal = 1;
                break;
            }
        }

        if (isFinal)
        {
            printf("D%d ", i);
        }
    }

    printf("\n");

    return 0;
}

/*
Enter number of states: 3
Enter number of input symbols: 2
Enter input symbols:
a b

Enter transition matrix for symbol 'a':
1 1 0
0 0 0
0 0 0

Enter transition matrix for symbol 'b':
1 0 0
0 0 1
0 0 0

Enter number of final states: 1
Enter final states:
2
*/