#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define kugiri() printf("---------------------------------\n")

typedef struct singlel {
    int key;
    struct singlel *next;
} Singlel;

Singlel *S_HEAD = NULL;
Singlel *S_TAIL = NULL;

Singlel *newNode(void) {
    return (Singlel *)malloc(sizeof(Singlel));
}

void addNode(int x) {
    Singlel *new = newNode();
    if (new == NULL) return;
    new->key = x;
    if (S_HEAD == NULL) {
        S_HEAD = new;
    } else {
        S_TAIL->next = new;
    }
    new->next = NULL;
    S_TAIL = new;
    return;
}

void printSinglel(void) {
    Singlel *current = S_HEAD;
    while (current != NULL) {
        printf("now : %lx\n", (long)current);
        printf("key : %d\n", current->key);
        printf("next: %lx\n", (long)current->next);
        kugiri();
        current = current->next;
    }
    return;
}

void printBin(int x) {
    int A[64];
    int i, j;
    for (i = 0; x != 0; i++) {
        A[i] = x & 1;
        x >>= 1;
    }
    for (j = i - 1; j >= 0; j--) 
        printf("%d", A[j]);
    putchar('\n');
    return;
}

int main(void) {
    srand((unsigned)time(NULL));
    int i, r;
    for (i = 0; i < 10; i++) {
        r = rand() % 100;
        addNode(r);
    }
    printSinglel();
    return 0;
}