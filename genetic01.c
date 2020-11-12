#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>

#include "genetic01.h"
#include "sort01.h"

// global variables

// functions

int sigmoid_pt(float *xp) {
    (*xp) = sigmoidFloat(*xp);
    return 0;
}

// float -0.5 ~ 0.5
float frand(void) {
    return (float)rand() / RAND_MAX - 0.5;
}

int board2array(Board src, int *dst) {
    int i;
    for (i = 0; i < 64; i++) {
        dst[i] = getKoma(src, i * 2);
    }
    return 0;
}

// black: +1, empty: 0, white: -1
// add an element for bias
void board2arraySymmetryPlus(Board src, int dst[MASU_NUM + 1]) {
    board2arraySymmetry(src, dst);
    // for bias
    dst[MASU_NUM] = 1;
}

// convert weight array to Prm1L
void array2Prm1L(float src[PRM1L_LEN], Prm1L *dst) {
    int c = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= MASU_NUM; j++) {
            dst->weight1[i][j] = src[c];
            c++;
        }
    }
    for (int i = 0; i < 8; i++) {
        dst->weight2[i] = src[c];
        c++;
    }
}

// convert Prm1L to a weight array
void Prm1L2array(Prm1L *src, float dst[PRM1L_LEN]) {
    int c = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j <= MASU_NUM; j++) {
            dst[c] = src->weight1[i][j];
            c++;
        }
    }
    for (int i = 0; i < 8; i++) {
        dst[c] = src->weight2[i];
        c++;
    }
    printDecimal(c);
}

int checkParam(Param pr) {
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        printf("1-%02d: ", i);
        for (j = 0; j < 64; j++) {
            printf("%4.2f ", pr.weight1[i][j]);
        }
        printf("\n%4.2f\n", pr.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        printf("2-%02d: ", i);
        for (j = 0; j < 32; j++) {
            printf("%4.2f ", pr.weight2[i][j]);
        }
        printf("\n%4.2f\n", pr.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        printf("3-%02d: ", i);
        for (j = 0; j < 16; j++) {
            printf("%4.2f ", pr.weight3[i][j]);
        }
        printf("\n%4.2f\n", pr.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        printf("4-%02d: ", i);
        for (j = 0; j < 8; j++) {
            printf("%4.2f ", pr.weight4[i][j]);
        }
        printf("\n%4.2f\n", pr.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        printf("5-%02d: ", i);
        for (j = 0; j < 4; j++) {
            printf("%4.2f ", pr.weight5[i][j]);
        }
        printf("\n%4.2f\n", pr.bias5[i]);
    } // sixth layer
    printf("6: ");
    for (j = 0; j < 2; j++) {
        printf("%4.2f ", pr.weight6[j]);
    }
    putchar('\n');
    return 0;
}

// show parameters (for Prm1L)
void showPrm1L(Prm1L pr) {
    printString("weight1:");
    for (int i = 0; i < 8; i++) {
        //printFloatArray(pr.weight1[i], MASU_NUM + 1);
        printFloatArrayPart(pr.weight1[i], MASU_NUM + 1);
    }
    printString("weight2:");
    printFloatArray(pr.weight2, 8);
}

// randomly determine parameter
int paramRand(Param *prp) {
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            (*prp).weight1[i][j] = frand();
        }
        (*prp).bias1[i] = frand();
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            (*prp).weight2[i][j] = frand();
        }
        (*prp).bias2[i] = frand();
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            (*prp).weight3[i][j] = frand();
        }
        (*prp).bias3[i] = frand();
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            (*prp).weight4[i][j] = frand();
        }
        (*prp).bias4[i] = frand();
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            (*prp).weight5[i][j] = frand();
        }
        (*prp).bias5[i] = frand();
    } // sixth layer
    for (j = 0; j < 2; j++) {
        (*prp).weight6[j] = frand();
    }
    return 0;
}

// create random parameters (for Prm1L)
void randPrm1L(Prm1L *prp) {
    float tmp[PRM1L_LEN];
    randWeightArray(tmp, PRM1L_LEN);
    array2Prm1L(tmp, prp);
}

// generation initialization
int familyRand(Family *fmp) {
    int i;
    for (i = 0; i < POPULATION; i++) {
        paramRand((*fmp).prms + i);
    }
    return 0;
}

// calculate point
// the more advantageous to black, the higher the score
float evaluation(Board b, Param pr) {
    int i, j;
    int pa0[64];
    float pa1[32], pa2[16], pa3[8], pa4[4], pa5[2];
    // output point
    float pa6 = 0;
    // input array
    board2array(b, pa0);
    // first layer
    for (i = 0; i < 32; i++) {
        // initialization
        pa1[i] = pr.bias1[i];
        for (j = 0; j < 64; j++)
            pa1[i] += pa0[j] * pr.weight1[i][j];
        sigmoid_pt(pa1 + i);
    } // second layer
    for (i = 0; i < 16; i++) {
        pa2[i] = pr.bias2[i];
        for (j = 0; j < 32; j++)
            pa2[i] += pa1[j] * pr.weight2[i][j];
        sigmoid_pt(pa2 + i);
    } // third layer
    for (i = 0; i < 8; i++) {
        pa3[i] = pr.bias3[i];
        for (j = 0; j < 16; j++)
            pa3[i] += pa2[j] * pr.weight3[i][j];
        sigmoid_pt(pa3 + i);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        pa4[i] = pr.bias4[i];
        for (j = 0; j < 8; j++)
            pa4[i] += pa3[j] * pr.weight4[i][j];
        sigmoid_pt(pa4 + i);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        pa5[i] = pr.bias5[i];
        for (j = 0; j < 4; j++)
            pa5[i] += pa4[j] * pr.weight5[i][j];
        sigmoid_pt(pa5 + i);
    } // sixth layer
    pa6 = 0;
    for (j = 0; j < 2; j++)
        pa6 += pa5[j] * pr.weight6[j];
    return pa6;
}

Board getBestBoard(Board *next_boards, int next_count, int color, Param prm) {
    float mx_point = -FLT_MAX;
    float t_point;
    int i;
    Board best_board;
    // check all boards
    for (i = 0; i < next_count; i++) {
        // evaluate a board
        t_point = evaluation(next_boards[i], prm);
        // if the color is white, reverse the sign
        if (color == 0b10) t_point = -t_point;
        if (mx_point < t_point) {
            // update
            mx_point = t_point;
            best_board = next_boards[i];
        }
    }
    return best_board;
}

// return winnter
// draw: 0, black: 1, white: 2
int oneToOne(Param sente, Param gote) {
    int winner;
    int cpa[NEXT_MAX];
    Board nba[NEXT_MAX];
    int kc[3];
    int pass = 0;
    int nc;
    // black
    int turn = 0b01;
    Board main_board = START;
    while (1) {
        nc = canPutPP(main_board, turn, cpa, nba, kc);
        //showBoard(main_board);
        // cannot put a piece anywhere
        if (nc == 0) {
            // cannot do anything each other
            if (pass) {
                break;
            }
            // pass
            turn ^= 0b11;
            pass = 1;
            continue;
        }
        pass = 0;
        // black
        if (turn == 0b01) {
            main_board = getBestBoard(nba, nc, 0b01, sente);
        } // white
        else {
            main_board = getBestBoard(nba, nc, 0b10, gote);
        }
        // next turn;
        turn ^= 0b11;
    }
    //showBoard(main_board);
    // the winner is white
    if (kc[1] < kc[2]) winner = 2;
    // black
    else if (kc[1] > kc[2]) winner = 1;
    // draw
    else winner = 0;
    return winner;
}

int leagueMatch(Family fml) {
    int i, j, k;
    // index, color, (d, w, l)
    int result[POPULATION][2][3];
    // all zero
    for (i = 0; i < POPULATION; i++)
        for (j = 0; j < 2; j++)
            for (k = 0; k < 3; k++)
                result[i][j][k] = 0;
    // black index
    for (i = 0; i < POPULATION; i++) {
    //for (i = 0; i < 10; i++) {
        //printf("\ai = %d / %d\n", i, POPULATION);
        // white index
        for (j = 0; j < POPULATION; j++) {
        //for (j = 0; j < 10; j++) {
            switch(oneToOne(fml.prms[i], fml.prms[j])) {
                // black won
                case 1:
                    result[i][0][1]++;
                    result[j][1][2]++;
                    break;
                // white won
                case 2:
                    result[i][0][2]++;
                    result[j][1][1]++;
                    break;
                // draw
                default:
                    result[i][0][0]++;
                    result[j][1][0]++;
            }
        }
    }
    // show result
    for (i = 0; i < POPULATION; i++) {
        printf("no.%02d: ", i);
        for (j = 0; j < 2; j++) {
            if (j) printf("w: ");
            else printf("b: ");
            for (k = 0; k < 3; k++) {
                printf("%2d ", result[i][j][k]);
            }
        }
        putchar('\n');
    }
    return 0;
}

// win: +2, draw: +1, lose: 0
int leagueMatchSimple(Family fml, int *result) {
    int i, j, k;
    // all zero
    zeros(result, POPULATION);
    // black index
    for (i = 0; i < POPULATION; i++) {
        //printf("\ai = %d / %d", i, POPULATION);
        // white index
        for (j = 0; j < POPULATION; j++) {
            if (i == j) continue;
            switch(oneToOne(fml.prms[i], fml.prms[j])) {
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
    // show results
    /*
    for (i = 0; i < POPULATION; i++) {
        printf("no.%02d: %3d\n", i, result[i]);
    }
    */
    return 0;
}

int readResultFile(int *result, int r_size, char *fnamel) {
    FILE *fp;
    // read file
    if ((fp = fopen(fnamel, "rb")) == NULL)
        printf("\a%s cannot be opened\n", fnamel);
    else {
        fread(result, r_size, 1, fp);
        fclose(fp);
    }
    return 0;
}

// choose survivors
// shallow copy?
int getSurvivor(Family *fmlp, Param *survivors) {
    int i, j;
    int result[POPULATION];
    int number[POPULATION];
    float dist;
    // number = {0, 1, 2, ...}
    indices(number, POPULATION);
    // game!
    leagueMatchSimple(*fmlp, result);
    // use data (for test)
    //readResultFile(result, sizeof result, "temporary_storage000.bin");
    // sort
    quicksortDD(result, number, 0, POPULATION - 1);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < SURVIVE_NUM; i++) {
        j = number[i];
        printf("%3d", j + 1);
        if (j < 10) printf("(p)");
        else printf("(c)");
        printf(" -> ");
        printf("%2d: %3dpt\n", i + 1, result[i]);
        survivors[i] = (*fmlp).prms[j];
    }
    printf("worst: %3dpt\n", result[99]);
    //putchar('\n');
    // calculate the distance between the previous top and the current top
    dist = paramDistance((*fmlp).prms[0], survivors[0]);
    printf("distance: %6.4f\n", dist);
    return 0;
}

float faverage(float f1, float f2) {
    return (f1 + f2) / 2;
}

Param makeChildAverage(Param mother, Param father) {
    Param child;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            child.weight1[i][j] = faverage(mother.weight1[i][j], father.weight1[i][j]);
        }
        child.bias1[i] = faverage(mother.bias1[i], father.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            child.weight2[i][j] = faverage(mother.weight2[i][j], father.weight2[i][j]);
        }
        child.bias2[i] = faverage(mother.bias2[i], father.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            child.weight3[i][j] = faverage(mother.weight3[i][j], father.weight3[i][j]);
        }
        child.bias3[i] = faverage(mother.bias3[i], father.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            child.weight4[i][j] = faverage(mother.weight4[i][j], father.weight4[i][j]);
        }
        child.bias4[i] = faverage(mother.bias4[i], father.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            child.weight5[i][j] = faverage(mother.weight5[i][j], father.weight5[i][j]);
        }
        child.bias5[i] = faverage(mother.bias5[i], father.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        child.weight6[j] = faverage(mother.weight6[j], father.weight6[j]);
    }
    return child;
}

float fcrossM(float f1, float f2) {
    // random number (0 ~ 199)
    int r = rand() % 200;
    // 0 ~ 98 (49.5%)
    if (r < 99) return f1;
    // 99 ~ 197 (49.5%)
    if (r < 198) return f2;
    // mutation!!
    // 198 or 199 (1%)
    return frand();
}

Param makeChildCrossM(Param mother, Param father) {
    Param child;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            child.weight1[i][j] = fcrossM(mother.weight1[i][j], father.weight1[i][j]);
        }
        child.bias1[i] = fcrossM(mother.bias1[i], father.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            child.weight2[i][j] = fcrossM(mother.weight2[i][j], father.weight2[i][j]);
        }
        child.bias2[i] = fcrossM(mother.bias2[i], father.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            child.weight3[i][j] = fcrossM(mother.weight3[i][j], father.weight3[i][j]);
        }
        child.bias3[i] = fcrossM(mother.bias3[i], father.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            child.weight4[i][j] = fcrossM(mother.weight4[i][j], father.weight4[i][j]);
        }
        child.bias4[i] = fcrossM(mother.bias4[i], father.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            child.weight5[i][j] = fcrossM(mother.weight5[i][j], father.weight5[i][j]);
        }
        child.bias5[i] = fcrossM(mother.bias5[i], father.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        child.weight6[j] = fcrossM(mother.weight6[j], father.weight6[j]);
    }
    return child;
}

// square distance
float sqDist(float f1, float f2) {
    return (float)pow(f1 - f2, 2.0);
}

float paramDistance(Param p1, Param p2) {
    float dist = 0.0;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            dist += sqDist(p1.weight1[i][j], p2.weight1[i][j]);
        }
        dist += sqDist(p1.bias1[i], p2.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            dist += sqDist(p1.weight2[i][j], p2.weight2[i][j]);
        }
        dist += sqDist(p1.bias2[i], p2.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            dist += sqDist(p1.weight3[i][j], p2.weight3[i][j]);
        }
        dist += sqDist(p1.bias3[i], p2.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            dist += sqDist(p1.weight4[i][j], p2.weight4[i][j]);
        }
        dist += sqDist(p1.bias4[i], p2.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            dist += sqDist(p1.weight5[i][j], p2.weight5[i][j]);
        }
        dist += sqDist(p1.bias5[i], p2.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        dist += sqDist(p1.weight6[j], p2.weight6[j]);
    }
    dist /= PARAM_NUM;
    return (float)sqrt(dist);
}

int nextGeneration(int gene_num) {
    int i, j, count;
    char format[] = "generation%03d.bin";
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    Param parents[SURVIVE_NUM];
    FILE *fp;
    Family pgene, cgene;
    snprintf(fnamer, FILENAME_MAX, format, gene_num);
    snprintf(fnamew, FILENAME_MAX, format, gene_num + 1);
    printf("read file: %s\n", fnamer);
    printf("write file: %s\n", fnamew);
    // read parent generation
    if ((fp = fopen(fnamer, "rb")) == NULL) {
        printf("\a%s can't be opened\n", fnamer);
        return -1;
    } else {
        fread(&pgene, sizeof pgene, 1, fp);
        fclose(fp);
    }
    // check write file
    if ((fp = fopen(fnamew, "rb")) != NULL) {
        printf("\a%s exists. Can I overwrite it? (y): ", fnamew);
        if (getchar() == 'y') {
            fclose(fp);
        } else {
            fclose(fp);
            return -1;
        }
    }
    // ranking
    getSurvivor(&pgene, parents);
    // take over the top 10 directly
    for (count = 0; count < SURVIVE_NUM; count++)
        cgene.prms[count] = parents[count];
    // make children
    // 45 combinations, 2 children per couple
    for (i = 0; i < SURVIVE_NUM - 1; i++) {
        for (j = i + 1; j < SURVIVE_NUM; j++) {
            cgene.prms[count] = makeChildAverage(parents[i], parents[j]);
            count++;
            cgene.prms[count] = makeChildCrossM(parents[i], parents[j]);
            count++;
        }
    }
    //printIntDec(count);

    // write child generation
    if ((fp = fopen(fnamew, "wb")) == NULL) {
        printf("\a%s cannot be opened\n", fnamew);
        return -1;
    } else {
        fwrite(&cgene, sizeof cgene, 1, fp);
        fclose(fp);
    }
    return 0;
}

int testFunc(int argc, char *argv[]) {
    int i;
    int st = 57;
    int loop = 100;
    long st_tm, tm;
    // initial configuration
    START.board[1] = START_H;
    START.board[0] = START_L;
    // seed reset
    srand((unsigned)time(&st_tm));
    for (i = st; i < st + loop; i++) {
        nextGeneration(i);
        time(&tm);
        printf("elapsed time: %lds\n", tm - st_tm);
    }
    return 0;
}