#include<stdio.h>

#define MAX 20

int num_states, num_transitions;

struct Transition {
    int start;
    char symbol;
    int end;
};

struct Transition transitions[MAX * MAX];

int result[MAX][MAX];
int count[MAX];

// Add a state to the closure if not already present
int add_state(int original, int state){
    // check if state is already present in original
    for (int i = 0; i < count[original]; i++){
        if (result[original][i] == state){
            return 0;
        }
    }

    // If not already in it then add it into result
    result[original][count[original]++] = state;
    return 1;
}

// Recursive function to find epsilon closure
void find_closure(int original, int current){
    for (int i = 0; i < num_transitions; i++){
        if (transitions[i].start == current && 
        transitions[i].symbol == 'e'){
            int next = transitions[i].end;

            if (add_state(original, next)){
                find_closure(original, next);
            }
        }
    }
}

int main(){
    printf("Enter number of states:");
    scanf("%d", &num_states);

    printf("Enter number of transitions:");
    scanf("%d", &num_transitions);

    printf("Enter transitions (start symbol end):\n");
    for (int i = 0; i < num_transitions; i++){
        scanf("%d %c %d", &transitions[i].start, &transitions[i].symbol, &transitions[i].end);
    }

    for (int i = 0; i < num_states; i++){
        count[i] = 0;
        add_state(i, i);
        find_closure(i, i);
    }

    printf("\nEpsilon Closures:\n");

    for (int i = 0; i < num_states; i++){
        printf("e-closure(q%d) = { ", i);

        for (int j = 0; j < count[i]; j++){
            printf("q%d,", result[i][j]);
        }
        printf("}\n");
    }

    return 0;
}