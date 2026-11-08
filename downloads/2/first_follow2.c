
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 20

char production[MAX][MAX];
char first[26][MAX];
char follow[26][MAX];
int n;

// Add a symbol to a set without duplicates
int add(char set[], char ch) {
    if (strchr(set, ch))
        return 0;

    int len = strlen(set);
    set[len] = ch;
    set[len + 1] = '\0';
    return 1;
}

// Calculate FIRST of a symbol
void findFirst(char ch) {
    if (!isupper(ch))
        return;

    int index = ch - 'A';

    for (int i = 0; i < n; i++) {
        if (production[i][0] != ch)
            continue;

        char *rhs = production[i] + 2;
        int nullable = 1;

        for (int j = 0; rhs[j] != '\0'; j++) {
            char symbol = rhs[j];

            if (!isupper(symbol)) {
                if (symbol != '#')
                    add(first[index], symbol);
                else
                    add(first[index], '#');

                nullable = (symbol == '#');
                break;
            }

            findFirst(symbol);

            int next = symbol - 'A';

            for (int k = 0; first[next][k]; k++) {
                if (first[next][k] != '#')
                    add(first[index], first[next][k]);
            }

            if (!strchr(first[next], '#')) {
                nullable = 0;
                break;
            }
        }

        if (nullable)
            add(first[index], '#');
    }
}

// Calculate FOLLOW of all non-terminals
void findFollow() {
    add(follow[production[0][0] - 'A'], '$');

    int changed;

    do {
        changed = 0;

        for (int i = 0; i < n; i++) {
            char lhs = production[i][0];
            char *rhs = production[i] + 2;

            for (int j = 0; rhs[j]; j++) {
                if (!isupper(rhs[j]))
                    continue;

                int current = rhs[j] - 'A';
                int nullable = 1;

                for (int k = j + 1; rhs[k]; k++) {
                    char symbol = rhs[k];

                    if (!isupper(symbol)) {
                        if (symbol != '#')
                            changed |= add(follow[current], symbol);

                        nullable = (symbol == '#');
                        break;
                    }

                    int next = symbol - 'A';

                    for (int m = 0; first[next][m]; m++) {
                        if (first[next][m] != '#')
                            changed |= add(follow[current],
                                           first[next][m]);
                    }

                    if (!strchr(first[next], '#')) {
                        nullable = 0;
                        break;
                    }
                }

                if (nullable) {
                    int parent = lhs - 'A';

                    for (int k = 0; follow[parent][k]; k++)
                        changed |= add(follow[current],
                                       follow[parent][k]);
                }
            }
        }
    } while (changed);
}

int main() {
    printf("Enter number of productions: ");
    scanf("%d", &n);

    printf("Enter productions (example S=AB):\n");

    for (int i = 0; i < n; i++)
        scanf("%19s", production[i]);

    // Find FIRST
    for (int i = 0; i < n; i++)
        findFirst(production[i][0]);

    // Find FOLLOW
    findFollow();

    printf("\nFIRST and FOLLOW:\n");

    int printed[26] = {0};

    for (int i = 0; i < n; i++) {
        char ch = production[i][0];
        int index = ch - 'A';

        if (printed[index])
            continue;

        printed[index] = 1;

        printf("FIRST(%c) = { %s }\n", ch, first[index]);
        printf("FOLLOW(%c) = { %s }\n", ch, follow[index]);
    }

    return 0;
}
