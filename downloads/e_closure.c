#include <stdio.h>

#define MAX 100

int epsilon[MAX][MAX];
int visited[MAX];
int n;

/* Find epsilon closure of a state using DFS */
void epsilonClosure(int state)
{
    int stack[MAX];
    int top = -1;

    /* Start with the state itself */
    stack[++top] = state;
    visited[state] = 1;

    while (top != -1)
    {
        int current = stack[top--];

        /* Check all states reachable by epsilon */
        for (int i = 0; i < n; i++)
        {
            if (epsilon[current][i] == 1 && !visited[i])
            {
                visited[i] = 1;
                stack[++top] = i;
            }
        }
    }
}

int main()
{
    printf("Enter number of states: ");
    scanf("%d", &n);

    printf("\nEnter epsilon transition matrix:\n");
    printf("(1 if qi --epsilon--> qj, otherwise 0)\n\n");

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            scanf("%d", &epsilon[i][j]);
        }
    }

    printf("\nEpsilon Closures:\n");

    for (int state = 0; state < n; state++)
    {
        /* Reset visited array */
        for (int i = 0; i < n; i++)
            visited[i] = 0;

        epsilonClosure(state);

        printf("epsilon-closure(q%d) = { ", state);

        for (int i = 0; i < n; i++)
        {
            if (visited[i])
                printf("q%d ", i);
        }

        printf("}\n");
    }

    return 0;
}