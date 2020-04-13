#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#include <string.h>
#include "../array.h"

#define DN_ELEMENTS 1000000
#define N_BENCHMARKS 30

int N_ELEMENTS = DN_ELEMENTS;
int N_ELEMENTS_REMOVAL;;


typedef struct HeavyStruct {
    int a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v;
} HeavyStruct;

typedef struct HeavyStructArray {
    int length;
    int allocated;
    HeavyStruct *data;
} HeavyStructArray;


int
heavyStructArrayAdd(HeavyStructArray *a, HeavyStruct element) {
    // init empty array
    if (a->allocated == 0) {
        a->allocated = 128;
        a->data = malloc(sizeof(HeavyStruct) * a->allocated);
        a->data[0] = element;
        a->length++;
        return 0;
    }
    if (a->length < a->allocated) {
        int newIndex = a->length;
        a->data[a->length] = element;
        a->length++;
        return newIndex;
    }
    // no free space, allocate new memory
    int newIndex = a->length;
    a->allocated *= 2;
    a->data = realloc(
            a->data
            , a->allocated * sizeof(HeavyStruct)
    );
    a->data[a->length] = element;
    a->length++;
    return newIndex;
}

float genericArrayTime = 0;
float specializedArrayTime = 0;

void runBenchmarks() {
    float startTime;
    float endTime;
    float timeElapsed;

    startTime = (float)clock()/CLOCKS_PER_SEC;
    HeavyStructArray a = ARRAY_NEW;
    int k;;
    for (int i = 0 ; i < N_ELEMENTS ; i++) {
        HeavyStruct hs = {};
        ARRAY_ADD(a, hs, k);
    }
    for (int i = 0 ; i < N_ELEMENTS_REMOVAL ; i++) {
        ARRAY_REMOVE(a, 0);
    }
    endTime = (float)clock()/CLOCKS_PER_SEC;
    timeElapsed = endTime - startTime;
    genericArrayTime += timeElapsed;

    int r0 = rand() % a.length;
    int r1 = rand() % a.length;
    printf("Optim prevention: %d %d\n", a.data[r0].q, a.data[r1].d);
    printf("Generic array functions: %f\n", timeElapsed);
    free(a.data);

    startTime = (float)clock()/CLOCKS_PER_SEC;
    HeavyStructArray sa = ARRAY_NEW;
    for (int i = 0 ; i < N_ELEMENTS ; i++) {
        HeavyStruct hs = {};
        heavyStructArrayAdd(&sa, hs);
    }
    for (int i = 0 ; i < N_ELEMENTS_REMOVAL ; i++) {
        ARRAY_REMOVE(sa, 0);
    }
    endTime = (float)clock()/CLOCKS_PER_SEC;
    timeElapsed = endTime - startTime;
    specializedArrayTime += timeElapsed;

    r0 = rand() % a.length;
    r1 = rand() % a.length;
    printf("Optim prevention: %d %d\n", sa.data[r0].q, sa.data[r1].d);
    printf("Specialized array functions: %f\n", timeElapsed);
    free(sa.data);
}

int main() {
    srand(time(NULL));
    N_ELEMENTS += rand() % 50000;
    N_ELEMENTS_REMOVAL = rand() % 10 + 5;
    for (int i = 0 ; i < N_BENCHMARKS ; i++) {
        runBenchmarks();
    }
    printf("Averages:\n");
    printf("Generic array functions: %f\n", genericArrayTime / N_BENCHMARKS);
    printf("Specialized array functions: %f\n", specializedArrayTime / N_BENCHMARKS);
}
