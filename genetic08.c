// use roulette to decide next action!
// also use roulette selection instead of elite selection
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <time.h>

#include "genetic02.h"
#include "sort01.h"

// the file name format
#define FNAME_FORMAT "prm/sprm_rltrlt%03d.bin"

// choose survivors[10] from generation[100]
// and show match results
// select survivors by roulette
// use the points directly
void getSurvivorSprmRltRlt(const Sprm *generation, Sprm *survivors) {
    Board (*decNxt)(Board*, int, const Sprm*);
    decNxt = getBoardForBlackSimpleRoulette;
    // the array to store points
    int result[GENE_NUM];
    int number[GENE_NUM];
    int lucky[SURVIVE_NUM];
    // number = {0, 1, 2, ..., 99}
    indices(number, GENE_NUM);
    // game (the next board is decided by roulette)
    leagueMatchSprmFlex(decNxt, generation, result);
    // sort (descending order)
    // not always necessary
    // make the results easy to understand
    quicksortDD(result, number, 0, GENE_NUM - 1);
    // roulette selection!
    // don't allow duplication
    rouletteIntMltDep(result, GENE_NUM, lucky, SURVIVE_NUM);
    // view result
    printf("selected ranking:\n");
    printDecimalArray(lucky, SURVIVE_NUM);
    // save the survivors and view
    printf("the survived parameters view:\n");
    for (int i = 0; i < SURVIVE_NUM; i++) {
        survivors[i] = generation[number[lucky[i]]];
        printFloatArray(survivors[i].weight, SPRM_LEN);
    }
    checkSprmStatistics(survivors, SURVIVE_NUM);
}

// make next generation file
// select survivors by roulette
int nextGenerationSprmRltRlt(int gene_num, int safety) {
    int i, j, count;
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    // previous survivors
    Sprm parents[SURVIVE_NUM];
    FILE *fp;
    // the file name to be read
    snprintf(fnamer, FILENAME_MAX, FNAME_FORMAT, gene_num);
    // the file name to be written
    snprintf(fnamew, FILENAME_MAX, FNAME_FORMAT, gene_num + 1);
    // view the file names
    printf("read file : %s\n", fnamer);
    printf("write file: %s\n", fnamew);
    // read parents
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        printf("\a%s can't be opened\n", fnamer);
        return -1;
    }
    // opened!
    fread(parents, sizeof parents, 1, fp);
    fclose(fp);
    // allow overwriting
    if (!safety);
    // check the file to be written (can read?)
    else if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a%s exists. Do you overwrite it? (y\\n): ", fnamew);
        fclose(fp);
        // don't overwrite
        if (getchar() != 121) {
            while (getchar() != 10);
            printf("terminated\n");
            return -1;
        }
        if (getchar() != 10) {
            while (getchar() != 10);
            printf("terminated\n");
            return -1;
        }
    }
    // store a generation
    Sprm generation[GENE_NUM];

    // 10 parents copy (elite selection)
    for (count = 0; count < SURVIVE_NUM; count++)
        generation[count] = parents[count];

    // crossing! (use elite genes)
    // 45 combinations, 2 children per couple
    for (i = 0; i < SURVIVE_NUM - 1; i++) {
        for (j = i + 1; j < SURVIVE_NUM; j++) {
            // average
            generation[count] = makeChildAverageSprm(parents[i], parents[j]);
            count++;
            // cross (the mutation rate is 5%)
            generation[count] = makeChildCrossMSprm(parents[i], parents[j]);
            count++;
        }
    }
    // calcurate survivors
    Sprm survivors[SURVIVE_NUM];
    // battle! (the next board is decided by roulette)
    getSurvivorSprmRltRlt(generation, survivors);

    // write current survivors to the file
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        printf("\a%s cannot be opened\n", fnamew);
        return -1;
    }
    // opened!
    fwrite(survivors, sizeof survivors, 1, fp);
    fclose(fp);
    return 0;
}

int main(void) {
    // initialize global variables
    setIndexes();
    initBoard();
    makeSprmSample();

    int (*nGene)(int, int);
    nGene = nextGenerationSprmRltRlt;
    nextGenerationSprmLoopFlex(nGene, 0, 0, 100);
    return 0;
}