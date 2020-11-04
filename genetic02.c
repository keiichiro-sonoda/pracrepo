#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

#include "genetic02.h"
#include "sort01.h"
// not necessary
//#include "othello.h"

// global variables
// declarations are required
int INDEXES[MASU_NUM];
Sprm SAMP_PRM;

// functions

// print a simple parameter
void showSprm(Sprm pr) {
    for (int i = 0; i < MASU_NUM; i++) {
        printf("%5.2f ", pr.weight[INDEXES[i]]);
        if (i % 8 == 7)
            putchar(10);
    }
}

// print a simple parameter on one line
void showSprmOneLine(Sprm pr) {
    printFloatArray(pr.weight, SPRM_LEN);
}

// show a part of parameter array
void showFamilyPart(Sprm *pra) {
    // population is 5 or less
    if (POPULATION <= 5) {
        // show all
        for (int i = 0; i < POPULATION; i++)
            showSprmOneLine(pra[i]);
        return;
    }
    // population is greater than 5
    for (int i = 0; i < 3; i++)
        showSprmOneLine(pra[i]);
    printf("                                      ...\n");
    showSprmOneLine(pra[POPULATION - 1]);
    return;
}

// get the smallest value in an array
int getMinArray(const int *A, int n) {
    int min = 0x7fffffff;
    for (int i = 0; i < n; i++)
        min = getMin(A[i], min);
    return min;
}

// normalize an address
int normalAd(int ad) {
    int i, eq_ads[8], adm;
    eq_ads[0] = ad;
    eq_ads[4] = mirrorLRAd(ad);
    for (i = 0; i < 3; i++) {
        eq_ads[i + 1] = rotL90DegAd(eq_ads[i]);
        eq_ads[i + 5] = mirrorLRAd(eq_ads[i + 1]);
    }
    return getMinArray(eq_ads, 8);
}

// ad: normalized address
int ad2index(int ad) {
    if (ad < 16)
        return ad / 2;
    if (ad < 32)
        return ad / 2 - 5;
    if (ad < 48)
        return ad / 2 - 11;
    if (ad < 64)
        return ad / 2 - 18;
    return -1;
}

// -0.5 ~ 0.5
void randSprm(Sprm *prp) {
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        prp->weight[i] = (float)rand() / RAND_MAX - 0.5;
    }
}

// calculate average
Sprm makeChildAverageSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        child.weight[i] = (mother.weight[i] + father.weight[i]) / 2;
    }
    return child;
}

// give mutant rate
float fcrossMFlex(float a, float b, float rate) {
    // 0.0 ~ 1.0
    float r = (float)rand() / RAND_MAX;
    // mutant!
    if (r <= rate) {
        //printf("m!\n");
        return (float)rand() / RAND_MAX - 0.5;
    }
    r = (float)rand() / RAND_MAX;
    // 50 : 50
    if (r < 0.5) return a;
    return b;
}

// cross parameters?
// the mutation rate is 5%
Sprm makeChildCrossMSprm(Sprm mother, Sprm father) {
    Sprm child;
    int i;
    for (i = 0; i < SPRM_LEN; i++) {
        // rate 5%
        child.weight[i] = fcrossMFlex(mother.weight[i], father.weight[i], 0.05);
    }
    return child;
}

// mutate with a given probability
// otherwise copy
float copyOrMutation(float x, float mut_rate) {
    // equal is not necessary
    if ((float)rand() / RAND_MAX <= mut_rate) {
        // from -0.5 to 0.5
        //printf("mutated!\n");
        return (float)rand() / RAND_MAX - 0.5;
    }
    return x;
}

// single point crossover
// give 2 parameters, children's array, and mutation rate
// the number of children is 2
void singlePointCrossover(Sprm mother, Sprm father, Sprm *children, float mut_rate) {
    // a random number from 0 to 8
    int p = rand() % (SPRM_LEN - 1);
    //printDecimal(p);
    // from 0 to p
    for (int i = 0; i <= p; i++) {
        children[0].weight[i] = copyOrMutation(mother.weight[i], mut_rate);
        children[1].weight[i] = copyOrMutation(father.weight[i], mut_rate);
    }
    // from p + 1 to 9
    for (int i = p + 1; i < SPRM_LEN; i++) {
        children[0].weight[i] = copyOrMutation(father.weight[i], mut_rate);
        children[1].weight[i] = copyOrMutation(mother.weight[i], mut_rate);
    }
}

// convert from an address to the weight index?
void setIndexes(void) {
    int i, ad;
    for (i = 0; i < MASU_NUM; i++) {
        ad = i * 2;
        INDEXES[i] = ad2index(normalAd(ad));
    }
}

// caluculate point
float evaluationSimple(Board b, Sprm pr) {
    float pt = 0;
    int i;
    // board array
    int ba[MASU_NUM];
    // Board -> int array
    board2arraySymmetry(b, ba);
    //showDecimalArray(ba, MASU_NUM);
    for (i = 0; i < MASU_NUM; i++) {
        // calculate the inner product
        pt += ba[i] * pr.weight[INDEXES[i]];
    }
    return pt;
}

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
Board getBestBoardForBlackSimple(Board *next_boards, int n, const Sprm *prp) {
    float mx_point = -FLT_MAX;
    float t_point;
    int i;
    Board best_board;
    for (i = 0; i < n; i++) {
        // sign inversion!!
        t_point = -evaluationSimple(next_boards[i], *prp);
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    //printf("%5.2f\n", mx_point);
    return best_board;
}

// assume that the next turn is black
// n: the number of next boards
// use simple parameter
// decide next board by roulette
Board getBoardForBlackSimpleRoulette(Board *next_boards, int n, const Sprm *prp) {
    float exp_points[n];
    float s;
    int choosed;
    for (int i = 0; i < n; i++) {
        // evaluate all next boards
        // and calculate the power of e (to make numbers positive)
        // sign inversion!
        exp_points[i] = expf(-evaluationSimple(next_boards[i], *prp) * 10);
    }
    //printFloatArray(exp_points, n);
    s = sumFloat(exp_points, n);
    choosed = rouletteFloat(exp_points, n, s);
    return next_boards[choosed];
}

// return winner
int oneToOneNormalSprm(const Sprm *spp, const Sprm *gpp) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set turn
    int turn = 0b01;
    // set initial board
    Board main_board = START;
    while (1) {
        // calculate next
        n = nextBoardNormal2(main_board, nba, kc);
        //showBoard(main_board);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                //printf("end\n");
                break;
            }
            // pass
            //printf("pass\n");
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // black (first)
        if (turn == 0b01) {
            //printf("black\n");
            main_board = getBestBoardForBlackSimple(nba, n, spp);
        } // white (second)
        else {
            //printf("white\n");
            main_board = getBestBoardForBlackSimple(nba, n, gpp);
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    //printDecimal(dif);
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

// return winner
// give a function pointer as an argument
int oneToOneNormalSprmFlex(Board (*decNxt)(Board*, int, const Sprm*), const Sprm *spp, const Sprm *gpp) {
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int n, dif;
    // set turn
    int turn = 0b01;
    // set initial board
    Board main_board = START;
    while (1) {
        // calculate next
        n = nextBoardNormal2(main_board, nba, kc);
        //showBoard(main_board);
        // can't put a piece anywhere
        if (n == 0) {
            // can't do anything one another
            if (pass) {
                //printf("end\n");
                break;
            }
            // pass
            //printf("pass\n");
            swapNormalizeBoard(&main_board);
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // determine a next board
        // black (first)
        if (turn == 0b01) {
            //printf("black\n");
            main_board = decNxt(nba, n, spp);
        } // white (second)
        else {
            //printf("white\n");
            main_board = decNxt(nba, n, gpp);
        }
        // switch turn
        turn ^= 0b11;
    }
    // difference between black and white
    dif = kc[1] - kc[2];
    //printDecimal(dif);
    if (dif > 0) return turn;
    if (dif < 0) return turn ^ 0b11;
    // draw
    return 0;
}

// make first generation file
// give a file name format
// record all individuals!!
int makeFirstGeneFileFlex(const char *format) {
    FILE *fp;
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, format, 0);
    // check existence
    if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a\"%s\" exists. Do you overwrite it? (y\\n): ", fnamew);
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
    // open a file to write (or make a file)
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamew);
        return -1;
    }
    // can write
    Sprm pra[POPULATION];
    for (int i = 0; i < POPULATION; i++)
        randSprm(pra + i);
    printSize(pra);
    fwrite(pra, sizeof pra, 1, fp);
    fclose(fp);
    return 0;
}

// check parameter in a file
// give the file name format and generation number
void checkSprmFile(const char *format, int gene_num) {
    FILE *fp;
    // the file name to be read
    char fnamer[FILENAME_MAX];
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    printString(fnamer);
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // opened!
    Sprm pa[POPULATION];
    fread(&pa, sizeof pa, 1, fp);
    fclose(fp);
    // check some parameters
    showFamilyPart(pa);
}

// use Sprm[100]
// win: +2, draw: +1, lose: 0
void leagueMatchSimpleSprm(Sprm *generation, int *result) {
    int i, j, k;
    // all zero
    zeros(result, POPULATION);
    // black index
    for (i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, FAMILY_NUM);
        // white index
        for (j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOneNormalSprm(generation + i, generation + j)) {
                // black won
                case 1:
                    result[i] += 2;
                    break;
                // white won
                case 2:
                    result[j] += 2;
                    break;
                // draw
                default:
                    result[i]++;
                    result[j]++;
            }
        }
    }
}

// use Sprm[100]
// win: +2, draw: +1, lose: 0
// give a function pointer to decide the next board
void leagueMatchSprmFlex(Board (*decNxt)(Board*, int, const Sprm*), const Sprm *generation, int *result) {
    // set all elements to zero
    zeros(result, POPULATION);
    // black index
    for (int i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, FAMILY_NUM);
        // white index
        for (int j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOneNormalSprmFlex(decNxt, generation + i, generation + j)) {
                // black won
                case 1:
                    result[i] += 2;
                    break;
                // white won
                case 2:
                    result[j] += 2;
                    break;
                // draw
                default:
                    result[i]++;
                    result[j]++;
            }
        }
    }
}

// calculate distance
float distSprm(Sprm p1, Sprm p2) {
    int i;
    float d = 0.0;
    for (i = 0; i < SPRM_LEN; i++) {
        // add square distance
        d += (float)pow(p1.weight[i] - p2.weight[i], 2.0);
    }
    // divided by number of parameters
    d /= SPRM_LEN;
    // return the square root
    return (float)sqrt(d);
}

// calculate means and standard deviation from some parameters
void checkSprmStatistics(const Sprm *pra, int pra_len) {
    int i, j;
    float mean[SPRM_LEN];
    float sd[SPRM_LEN];
    zerosFloat(mean, SPRM_LEN);
    zerosFloat(sd, SPRM_LEN);
    // calculate sum of each weight
    for (i = 0; i < pra_len; i++)
        for (j = 0; j < SPRM_LEN; j++)
            mean[j] += pra[i].weight[j];
    
    for (i = 0; i < pra_len; i++)
        mean[i] /= pra_len;
    
    for (i = 0; i < pra_len; i++)
        for (j = 0; j < SPRM_LEN; j++)
            sd[j] += powf(pra[i].weight[j] - mean[j], 2.0f);
    
    for (i = 0; i < pra_len; i++)
        sd[i] = sqrtf(sd[i] / pra_len);

    printf("mean: ");
    printFloatArray(mean, SPRM_LEN);
    printf("SD:   ");
    printFloatArray(sd, SPRM_LEN);
}

// choose survivors from Sprm[100]
// and show match results
void getSurvivorSprm(Sprm *generation, Sprm *survivors) {
    int i, j;
    int result[POPULATION];
    int number[POPULATION];
    float dist;
    // number = {0, 1, 2, ..., 99}
    for (i = 0; i < POPULATION; i++)
        number[i] = i;
    // game!
    leagueMatchSimpleSprm(generation, result);
    // sort (descending order)
    quicksortDD(result, number, 0, POPULATION - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < POPULATION; i++) {
        // rank 11 .. 99 aren't displayed
        if (10 <= i && i < POPULATION - 1) continue;
        // worst!
        if (i == POPULATION - 1)
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
    //printf("top parameters view:\n");
    //showSprm(survivors[0]);
}

// make next generation file
// give a function to choose survivors
int nextGenerationSprmFlex(void (*getSvr)(const Sprm*, Sprm*), const char *format, int gene_num, int safety) {
    int i, j, count;
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    // previous survivors
    Sprm parents[SURVIVE_NUM];
    FILE *fp;
    // the file name to be read
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    // the file name to be written
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
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
    Sprm generation[POPULATION];

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
    // battle!
    getSvr(generation, survivors);

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

// give a function to loop
void nextGenerationSprmLoopFlex(int (*nGene)(int, int), int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGene(i, safety) == -1)
            return;
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}

// give a function to loop
void nextGenerationSprmFlexLoopFlex(void (*getSvr)(const Sprm*, Sprm*), int (*nGeneF)(void(), const char*, int, int), const char *format, int safety, int st, int loop) {
    time_t t0, t1;
    // get start time
    time(&t0);
    for (int i = st; i < st + loop; i++) {
        // set the generation number as the seed
        srand((unsigned)i);
        if (nGeneF(getSvr, format, i, safety) == -1)
            return;
        // get time
        time(&t1);
        printf("elapsed time: %lds\n", t1 - t0);
    }
}

// make a sample of parameters
// set global variable "SAMP_PRM"
void makeSprmSample(void) {
    // use macro
    float spr[] = {SAMP_PRM_NUMS};
    for (int i = 0; i < SPRM_LEN; i++)
        SAMP_PRM.weight[i] = spr[i];
}

// copy the first generation
// give the destination file format
void copyFGFlex(const char *dst_format) {
    FILE *fp;
    // file name for reading (source)
    char fnamer[] = "prm/simple_prm000.bin";
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        // failed
        printf("%s can't be opened.\n", fnamer);
        return;
    }
    // opened!
    Sprm pa[SURVIVE_NUM];
    // temporary substitution
    fread(pa, sizeof pa, 1, fp);
    fclose(fp);
    // check the parameters
    for (int i = 0; i < SURVIVE_NUM; i++)
        printFloatArray(pa[i].weight, SPRM_LEN);
    // file name for writing (destination)
    char fnamew[FILENAME_MAX];
    snprintf(fnamew, FILENAME_MAX, dst_format, 0);
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

// for debugging
void test1(void) {
    // set global variable
    setIndexes();
    printDecimalArray(INDEXES, MASU_NUM);
    // set initial board
    initBoard();
    // test
    showBoard(START);
    //sortTest();
    checkSprmFile("prm/simple_prm%03d.bin", 0);
}