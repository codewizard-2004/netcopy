
#include <stdio.h>

#define MAX 20
#define MAX_DFA 1024

struct Transition {
    int start;
    char symbol;
    int end;
};

struct Transition transitions[400];

int num_states, num_transitions;
int dfa[MAX_DFA], dfa_count = 0;
int table[MAX_DFA][2];

// Find next DFA state for a given set and symbol
int move(int set, char symbol) {
    int next = 0;

    for (int i = 0; i < num_transitions; i++) {
        if (transitions[i].symbol == symbol &&
            (set & (1 << transitions[i].start))) {

            next |= (1 << transitions[i].end);
        }
    }

    return next;
}

// Print states in a set
void print_set(int set) {
    printf("{");

    for (int i = 0; i < num_states; i++) {
        if (set & (1 << i))
            printf("q%d ", i);
    }

    printf("}");
}

int main() {
    int start, current, next;

    printf("Enter number of NFA states: ");
    scanf("%d", &num_states);

    printf("Enter number of transitions: ");
    scanf("%d", &num_transitions);

    printf("Enter transitions (start symbol end):\n");

    for (int i = 0; i < num_transitions; i++) {
        scanf("%d %c %d",
              &transitions[i].start,
              &transitions[i].symbol,
              &transitions[i].end);
    }

    printf("Enter initial state: ");
    scanf("%d", &start);

    // Initial DFA state
    dfa[dfa_count++] = (1 << start);

    // Generate DFA states and transitions
    for (int i = 0; i < dfa_count; i++) {
        current = dfa[i];

        for (int j = 0; j < 2; j++) {
            char symbol = 'a' + j;
            next = move(current, symbol);

            table[i][j] = next;

            // Add new state if not already present
            int found = 0;

            for (int k = 0; k < dfa_count; k++) {
                if (dfa[k] == next) {
                    found = 1;
                    break;
                }
            }

            if (!found && dfa_count < MAX_DFA)
                dfa[dfa_count++] = next;
        }
    }

    // Print DFA transition table
    printf("\nDFA Transition Table:\n");
    printf("State\t\ta\t\tb\n");

    for (int i = 0; i < dfa_count; i++) {
        print_set(dfa[i]);
        printf("\t\t");

        print_set(table[i][0]);
        printf("\t\t");

        print_set(table[i][1]);
        printf("\n");
    }

    return 0;
}
