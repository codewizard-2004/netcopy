#include <stdio.h>

#define MAX_STATES 20
#define MAX_SYMBOLS 10

int n, m;

char symbols[MAX_SYMBOLS];

/* DFA transition table */
int transition[MAX_STATES][MAX_SYMBOLS];

/* 1 if final, 0 otherwise */
int finalState[MAX_STATES];

/* Current partition */
int group[MAX_STATES];

/* New partition */
int newGroup[MAX_STATES];


/* Check whether two states are equivalent
   according to the current partition */
int equivalent(int s1, int s2)
{
    for (int i = 0; i < m; i++)
    {
        if (group[transition[s1][i]] !=
            group[transition[s2][i]])
        {
            return 0;
        }
    }

    return 1;
}


/* Display the current partition */
void printPartition()
{
    int maxGroup = 0;

    for (int i = 0; i < n; i++)
    {
        if (group[i] > maxGroup)
            maxGroup = group[i];
    }

    printf("\nPartition:\n");

    for (int g = 0; g <= maxGroup; g++)
    {
        printf("{ ");

        for (int i = 0; i < n; i++)
        {
            if (group[i] == g)
                printf("q%d ", i);
        }

        printf("}\n");
    }
}


int main()
{
    /* -----------------------------------------
       Read DFA
       ----------------------------------------- */

    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("Enter number of input symbols: ");
    scanf("%d", &m);

    printf("Enter input symbols:\n");

    for (int i = 0; i < m; i++)
        scanf(" %c", &symbols[i]);


    /* -----------------------------------------
       Read transition table
       ----------------------------------------- */

    printf("\nEnter DFA transition table:\n");

    for (int i = 0; i < n; i++)
    {
        printf("For state q%d:\n", i);

        for (int j = 0; j < m; j++)
        {
            printf("  on '%c' -> q", symbols[j]);
            scanf("%d", &transition[i][j]);
        }
    }


    /* -----------------------------------------
       Read final states
       ----------------------------------------- */

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


    /* -----------------------------------------
       Initial partition
       ----------------------------------------- */

    /*
       Group 0 = non-final states
       Group 1 = final states
    */

    for (int i = 0; i < n; i++)
    {
        if (finalState[i])
            group[i] = 1;
        else
            group[i] = 0;
    }

    printf("\n====================================");
    printf("\nDFA MINIMIZATION");
    printf("\n====================================");

    printPartition();


    /* -----------------------------------------
       Partition refinement
       ----------------------------------------- */

    while (1)
    {
        int numberOfGroups = 0;

        /*
           Initially no state belongs to a new group
        */

        for (int i = 0; i < n; i++)
            newGroup[i] = -1;


        /*
           Construct new partition
        */

        for (int i = 0; i < n; i++)
        {
            if (newGroup[i] == -1)
            {
                /*
                   Put state i in a new group
                */

                newGroup[i] = numberOfGroups;

                /*
                   Compare with other states
                */

                for (int j = i + 1; j < n; j++)
                {
                    /*
                       They must first belong to
                       the same old group.
                    */

                    if (group[i] == group[j] &&
                        equivalent(i, j))
                    {
                        newGroup[j] = numberOfGroups;
                    }
                }

                numberOfGroups++;
            }
        }


        /*
           Check whether partition changed
        */

        int changed = 0;

        for (int i = 0; i < n; i++)
        {
            if (group[i] != newGroup[i])
            {
                changed = 1;
                break;
            }
        }


        /*
           Update partition
        */

        for (int i = 0; i < n; i++)
            group[i] = newGroup[i];


        printPartition();


        /*
           If there is no change,
           minimization is complete.
        */

        if (!changed)
            break;
    }


    /* -----------------------------------------
       Determine number of minimized states
       ----------------------------------------- */

    int minimizedStates = 0;

    for (int i = 0; i < n; i++)
    {
        if (group[i] + 1 > minimizedStates)
            minimizedStates = group[i] + 1;
    }


    /* -----------------------------------------
       Display minimized DFA
       ----------------------------------------- */

    printf("\n====================================");
    printf("\nMINIMIZED DFA");
    printf("\n====================================\n");


    /*
       Display states
    */

    printf("\nStates:\n");

    for (int g = 0; g < minimizedStates; g++)
    {
        printf("D%d = { ", g);

        for (int i = 0; i < n; i++)
        {
            if (group[i] == g)
                printf("q%d ", i);
        }

        printf("}\n");
    }


    /*
       Display transition table
    */

    printf("\nTransition Table:\n");

    for (int g = 0; g < minimizedStates; g++)
    {
        int representative = -1;

        /*
           Find representative state
        */

        for (int i = 0; i < n; i++)
        {
            if (group[i] == g)
            {
                representative = i;
                break;
            }
        }

        printf("\nD%d:\n", g);

        for (int s = 0; s < m; s++)
        {
            int destination =
                group[transition[representative][s]];

            printf("  on '%c' -> D%d\n",
                   symbols[s],
                   destination);
        }
    }


    /*
       -----------------------------------------
       Display final states
       -----------------------------------------
    */

    printf("\nFinal states: ");

    for (int g = 0; g < minimizedStates; g++)
    {
        for (int i = 0; i < n; i++)
        {
            if (group[i] == g && finalState[i])
            {
                printf("D%d ", g);
                break;
            }
        }
    }

    printf("\n");

    return 0;
}