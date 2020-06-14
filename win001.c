#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define kugiri() printf("---------------------------------\n")
#define printSize(x) printf("%ld\n", sizeof x)
#define printDecimal(x) printf("%d\n", x)
#define arrayLength(A) sizeof(A) / sizeof(A[0])

typedef struct singlel {
    int key;
    struct singlel *next;
} Singlel;

typedef struct doublel {
    struct doublel *prev;
    int key;
    struct doublel *next;
} Doublel;

Singlel *S_HEAD = NULL;
Singlel *S_TAIL = NULL;

Doublel *D_HEAD = NULL;
Doublel *D_TAIL = NULL;

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

Doublel *newNodeDoublel(void) {
    return (Doublel *)malloc(sizeof(Doublel));
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

void addNodeDoublel(int x) {
    Doublel *new = newNodeDoublel();
    if (new == NULL) {
        printf("cannot malloc.\n");
        return;
    }
    new->key = x;
    if (D_HEAD == NULL) {
        D_HEAD = new;
        new->prev = NULL;
    } else {
        D_TAIL->next = new;
        new->prev = D_TAIL;
    }
    new->next = NULL;
    D_TAIL = new;
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

void printDoublel(void) {
    Doublel *current = D_HEAD;
    while (current != NULL) {
        printf("prev: %x\n", current->prev);
        printf("key : %d\n", current->key);
        printf("next: %d\n", current->next);
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

int heapParent(int i) {
    return (i - 1) >> 1;
}

int heapLeft(int i) {
    return (i << 1) + 1;
}

int heapRight(int i) {
    return i + 1 << 1;
}

void swap(int *A, int i, int j) {
    int tv = A[i];
    A[i] = A[j];
    A[j] = tv;
    return;
}

void maxHeapify(int *A, int i, int n) {
    int largest = i;
    int l = heapLeft(i);
    int r = heapRight(i);
    if (l < n && A[l] > A[i])
        largest = l;
    if (r < n && A[r] > A[largest])
        largest = r;
    if (largest != i) {
        swap(A, i, largest);
        //printf("%d %d\n", A[i], A[largest]);
        maxHeapify(A, largest, n);
    }
    return;
}

void buildMaxHeap(int *A, int n) {
    int i;
    for (i = heapParent(n - 1); i >= 0; i--) {
        //printDecimal(i);
        maxHeapify(A, i, n);
    }
    return;
}

void heapsort(int *A, int n) {
    int i;
    buildMaxHeap(A, n);
    for (i = n - 1; i >= 1; i--) {
        swap(A, 0, i);
        maxHeapify(A, 0, i);
    }
    return;
}

int main(void) {
    srand((unsigned)time(NULL));
    int dla[] = {9, 16, 4, 1};
    int dll = arrayLength(dla);
    int i;
    for (i = 0; i < dll; i++)
        addNodeDoublel(dla[i]);
    printDoublel();
    return 0;
}