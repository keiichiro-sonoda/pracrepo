#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define kugiri() printf("---------------------------------\n")
#define printSize(x) printf("%ld\n", sizeof x)

typedef struct singlel {
    int key;
    struct singlel *next;
} Singlel;

Singlel *S_HEAD = NULL;
Singlel *S_TAIL = NULL;

void printIntArray(const int *A, int n) {
    int i;
    for (i = 0; i < n; i++)
        printf("%d ", A[i]);
    putchar('\n');
    return;
}

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
        printf("now : %x\n", current);
        printf("key : %d\n", current->key);
        printf("next: %x\n", current->next);
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

int *makeArrayRand(int n) {
    int *A;
    int i;
    A = (int *)malloc(n * sizeof(int));
    for (i = 0; i < n; i++) {
        A[i] = rand() % 100;
    }
    return A;
}

void insertionSortRecursive(int *A, int n) {
    if (n > 1) {
        insertionSortRecursive(A, n - 1);
        int key = A[n - 1];
        int i;
        for (i = n - 2; i >= 0 && A[i] > key; i--)
            A[i + 1] = A[i];
        A[i + 1] = key;
    }
    return;
}

int main(void) {
    srand((unsigned)time(NULL));
    int i, r;
    int l = 16;
    int *sample1 = makeArrayRand(l);
    printIntArray(sample1, l);
    insertionSortRecursive(sample1, l);
    printIntArray(sample1, l);
    printSize(S_HEAD);
    for (i = 0; i < 10; i++) {
        r = rand() % 100;
        addNode(r);
    }
    printSinglel();
    return 0;
}