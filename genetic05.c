// calculate goodness by playing against random AI
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genetic02.h"
#include "sort01.h"

// copy the first generation
// for vs random AI
void copyFirstGeneration2(void) {
    FILE *fp;
    // file name for reading
    char fnamer[] = "prm/simple_prm000.bin";
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // opened!
    Sprm pa[SURVIVE_NUM];
    fread(pa, sizeof pa, 1, fp);
    fclose(fp);
    // check the parameters
    for (int i = 0; i < SURVIVE_NUM; i++)
        printFloatArray(pa[i].weight, SPRM_LEN);
    // file name for writing
    char fnamew[] = "prm/sprm_vsrand000.bin";
    // open a file to write (or make a file)
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamew);
        return;
    }
    // opened!
    fwrite(pa, sizeof pa, 1, fp);
    // close
    fclose(fp);
}

// use Sprm[100]
// win: +2, draw: +1, lose: 0
// play against random AI 100 times for each parameter
void calcGoodnessVSRandom(Sprm *generation, int *result) {
    int i, j;
    // reset result
    zeros(result, GENE_NUM);
    // black index
    for (i = 0; i < GENE_NUM; i++) {
        for (j = 0; j < 50; j++) {
            // parameter's turn is black (50 times)
            switch(sprmVSRandomNormal(generation + i, 1)) {
                // black won
                case 1:
                    result[i] += 2;
                    break;
                // draw
                case 0:
                    result[i]++;
                // white won
                default:
                    ;
            }
            // parameter's turn is white (50 times)
            switch(sprmVSRandomNormal(generation + i, 2)) {
                // white won
                case 2:
                    result[i] += 2;
                    break;
                // draw
                case 0:
                    result[i]++;
                // black won
                default:
                    ;
            }
        }
    }
}

// choose survivors from Sprm[100]
// and show match results
// use calcGoodnessVSRandom()
void getSurvivorSprmVSRandom(Sprm *generation, Sprm *survivors) {
    int i, j;
    int result[GENE_NUM];
    int number[GENE_NUM];
    float dist;
    // number = {0, 1, 2, ..., 99}
    for (i = 0; i < GENE_NUM; i++)
        number[i] = i;
    // game! (vs random AI)
    calcGoodnessVSRandom(generation, result);
    // sort (descending order)
    quicksortDD(result, number, 0, GENE_NUM - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < GENE_NUM; i++) {
        // rank 11 .. 99 aren't displayed
        if (10 <= i && i < GENE_NUM - 1) continue;
        // worst!
        if (i == GENE_NUM - 1)
            printf("        ...\n");
        // winner's index (or worst index)
        j = number[i];
        printf("%3d", j + 1);
        if (j < 10) printf("(p)");
        else printf("(c)");
        printf(" -> ");
        printf("%3d: %3dpt\n", i + 1, result[i]);
        // record winners
        if (i < 10)
            survivors[i] = generation[j];
    }
    // calculate the distance between the previous top and the current top
    dist = distSprm(survivors[0], generation[0]);
    printf("distance: %6.4f\n", dist);
    // print top 10 parameters
    printString("top ten parameters:");
    for (i = 0; i < SURVIVE_NUM; i++) {
        printf("%3d: ", i + 1);
        printFloatArray(survivors[i].weight, SPRM_LEN);
    }
    // print worst parameter
    printString("worst parameter:");
    printf("%3d: ", GENE_NUM);
    printFloatArray(generation[GENE_NUM - 1].weight, SPRM_LEN);
}

// make next generation file
// use getSurvivorSprmVSRandom()
int nextGenerationSprmVSRandom(int gene_num) {
    int i, j, count;
    char format[] = "prm/sprm_vsrand%03d.bin";
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    // previous survivors
    Sprm parents[SURVIVE_NUM];
    FILE *fp;
    // read file name
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    // write file name
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    // view
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
    // check write file (can read?)
    if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a%s exists. Can I overwrite it? (y): ", fnamew);
        fclose(fp);
        // don't overwrite
        if (getchar() != 'y') {
            printf("terminated\n");
            return -1;
        }
    }
    // make children!
    Sprm generation[GENE_NUM];
    // 10 parents are copied
    for (count = 0; count < SURVIVE_NUM; count++) {
        generation[count] = parents[count];
    }
    // make children
    // 45 combinations, 2 children per couple
    for (i = 0; i < SURVIVE_NUM - 1; i++) {
        for (j = i + 1; j < SURVIVE_NUM; j++) {
            // average
            generation[count] = makeChildAverageSprm(parents[i], parents[j]);
            count++;
            // cross (mutation rate is 5%)
            generation[count] = makeChildCrossMSprm(parents[i], parents[j]);
            count++;
        }
    }
    // calcurate survivors
    Sprm survivors[SURVIVE_NUM];
    // battle! (vs random AI)
    getSurvivorSprmVSRandom(generation, survivors);

    // write current survivors
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        printf("\a%s cannot be opened\n", fnamew);
        return -1;
    }
    // opened!
    fwrite(survivors, sizeof survivors, 1, fp);
    fclose(fp);

    return 0;
}

// loop several times
// for vs random files
void nextGenerationSprmVSRL(int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        nextGenerationSprmVSRandom(i);
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}

int main(void) {
    // initial configurations
    initBoard();
    setIndexes();
    srand((unsigned)time(NULL));

    nextGenerationSprmVSRL(100, 200);
    return 0;
}