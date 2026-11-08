
#include <stdio.h>

#define MAX 20

int num_states, num_transitions;

struct Transition {
    int start;
    char symbol;
    int end;
};

struct Transition transitions[MAX * MAX];

int closure[MAX][MAX];
int count[MAX];

// Original and new final states
int final[MAX], newFinal[MAX] = {0};
int num_final, start;

// Add a state to the closure if not already present
int add_state(int original, int state) {
    for (int i = 0; i < count[original]; i++) {
        if (closure[original][i] == state)
            return 0;
    }

    closure[original][count[original]++] = state;
    return 1;
}

// Recursive function to find epsilon closure
void find_closure(int original, int current) {
    for (int i = 0; i < num_transitions; i++) {
        if (transitions[i].start == current &&
            transitions[i].symbol == 'e') {

            int next = transitions[i].end;

            if (add_state(original, next)) {
                find_closure(original, next);
            }
        }
    }
}

// NEW: Convert epsilon-NFA to NFA
void convert_to_nfa() {
    printf("\nEquivalent NFA Transitions:\n");

    for (int i = 0; i < num_states; i++) {
        for (char symbol = 'a'; symbol <= 'z'; symbol++) {
            
            int nfa_result[MAX] = {0};
            int nfa_count = 0;

            // Explore epsilon closure of state i
            for (int j = 0; j < count[i]; j++) {
                int current = closure[i][j];

                // Find transitions on the input symbol
                for (int k = 0; k < num_transitions; k++) {
                    if (transitions[k].start == current &&
                        transitions[k].symbol == symbol) {

                        int dest = transitions[k].end;

                        // Add epsilon closure of destination
                        for (int l = 0; l < count[dest]; l++) {
                            int next = closure[dest][l];

                            // Avoid duplicate destinations
                            int found = 0;
                            for (int p = 0; p < nfa_count; p++) {
                                if (nfa_result[p] == next) {
                                    found = 1;
                                    break;
                                }
                            }

                            if (!found)
                                nfa_result[nfa_count++] = next;
                        }
                    }
                }
            }

            if (nfa_count > 0) {
                printf("q%d --%c--> { ", i, symbol);

                for (int j = 0; j < nfa_count; j++)
                    printf("q%d ", nfa_result[j]);

                printf("}\n");
            }
        }
    }
}

// NEW: Find final states of equivalent NFA
void find_final_states() {
    for (int i = 0; i < num_states; i++) {
        for (int j = 0; j < count[i]; j++) {
            for (int k = 0; k < num_final; k++) {
                if (closure[i][j] == final[k])
                    newFinal[i] = 1;
            }
        }
    }
}

int main() {
    printf("Enter number of states: ");
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

    // NEW: Read initial and final states
    printf("Enter initial state: ");
    scanf("%d", &start);

    printf("Enter number of final states: ");
    scanf("%d", &num_final);

    printf("Enter final states: ");
    for (int i = 0; i < num_final; i++)
        scanf("%d", &final[i]);

    // Calculate epsilon closures (your original code)
    for (int i = 0; i < num_states; i++) {
        count[i] = 0;
        add_state(i, i);
        find_closure(i, i);
    }

    printf("\nEpsilon Closures:\n");

    for (int i = 0; i < num_states; i++) {
        printf("e-closure(q%d) = { ", i);

        for (int j = 0; j < count[i]; j++)
            printf("q%d ", closure[i][j]);

        printf("}\n");
    }

    // NEW: Convert and print equivalent NFA
    convert_to_nfa();

    // NEW: Determine new final states
    find_final_states();

    printf("\nInitial State: q%d\n", start);

    printf("Final States: ");
    for (int i = 0; i < num_states; i++) {
        if (newFinal[i])
            printf("q%d ", i);
    }

    printf("\n");

    return 0;
}
