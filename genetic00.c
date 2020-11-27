#include <float.h>
#include "sort01.h"
#include "genetic00.h"

// 盤面をそのまんま配列に変換
void board2array(Board src, int *dst) {
    for (int i = 0; i < MASU_NUM; i++)
        dst[i] = getKoma(src, i * 2);
}

// パラメータチェック
void checkParam(Param pr) {
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
}

// ランダムなパラメータを作る
// randomly determine parameter
void randParam(Param *prp) {
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            (*prp).weight1[i][j] = randWeight();
        }
        (*prp).bias1[i] = randWeight();
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            (*prp).weight2[i][j] = randWeight();
        }
        (*prp).bias2[i] = randWeight();
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            (*prp).weight3[i][j] = randWeight();
        }
        (*prp).bias3[i] = randWeight();
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            (*prp).weight4[i][j] = randWeight();
        }
        (*prp).bias4[i] = randWeight();
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            (*prp).weight5[i][j] = randWeight();
        }
        (*prp).bias5[i] = randWeight();
    } // sixth layer
    for (j = 0; j < 2; j++) {
        (*prp).weight6[j] = randWeight();
    }
}

// 配列ごと初期化
// generation initialization
void randFamily(Family *fmp) {
    for (int i = 0; i < POPULATION; i++)
        randParam((fmp->prms) + i);
}

// 評価値計算
// 活性化関数はシグモイド関数
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
        sigmoidFloatP(pa1 + i);
    } // second layer
    for (i = 0; i < 16; i++) {
        pa2[i] = pr.bias2[i];
        for (j = 0; j < 32; j++)
            pa2[i] += pa1[j] * pr.weight2[i][j];
        sigmoidFloatP(pa2 + i);
    } // third layer
    for (i = 0; i < 8; i++) {
        pa3[i] = pr.bias3[i];
        for (j = 0; j < 16; j++)
            pa3[i] += pa2[j] * pr.weight3[i][j];
        sigmoidFloatP(pa3 + i);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        pa4[i] = pr.bias4[i];
        for (j = 0; j < 8; j++)
            pa4[i] += pa3[j] * pr.weight4[i][j];
        sigmoidFloatP(pa4 + i);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        pa5[i] = pr.bias5[i];
        for (j = 0; j < 4; j++)
            pa5[i] += pa4[j] * pr.weight5[i][j];
        sigmoidFloatP(pa5 + i);
    } // sixth layer
    pa6 = 0;
    for (j = 0; j < 2; j++)
        pa6 += pa5[j] * pr.weight6[j];
    return pa6;
}

// 評価値計算して最高点だった盤面を返す
Board getBestBoard(Board *next_boards, int next_count, int color, Param prm) {
    float mx_point = -FLT_MAX;
    float t_point;
    Board best_board;
    // check all boards
    for (int i = 0; i < next_count; i++) {
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

// 初代一対一関数
// return winnter
// draw: 0, black: 1, white: 2
int oneToOne(Param sente, Param gote) {
    // 同じ型なら一行で定義しちゃう
    int winner, nc, pass, turn, kc[3], cpa[NEXT_MAX];
    Board nba[NEXT_MAX];
    pass = 0;
    // black
    turn = 0b01;
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

// リーグマッチ関数
// 最初は色とか保存しようとしてたっけ
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

// 勝点を計算するタイプのリーグ戦
// 今はこっちが主流
// win: +2, draw: +1, lose: 0
int leagueMatchSimple(Family fml, int *result) {
    int i, j;
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

// 生き残りを取得 (要するにエリート)
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
    if (readResultFile(result, sizeof result, "temporary_storage000.bin") < 0)
        return -1;
    // 乱択版クイックソートに変更
    // sort
    randomizedQuicksortDDAll(result, number, POPULATION);
    // show ranking
    printf("rank change\n");
    for (i = 0; i < ELITE_NUM; i++) {
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

// 平均化交叉
Param averageCrossover(Param mother, Param father) {
    Param child;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            child.weight1[i][j] = getAve(mother.weight1[i][j], father.weight1[i][j]);
        }
        child.bias1[i] = getAve(mother.bias1[i], father.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            child.weight2[i][j] = getAve(mother.weight2[i][j], father.weight2[i][j]);
        }
        child.bias2[i] = getAve(mother.bias2[i], father.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            child.weight3[i][j] = getAve(mother.weight3[i][j], father.weight3[i][j]);
        }
        child.bias3[i] = getAve(mother.bias3[i], father.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            child.weight4[i][j] = getAve(mother.weight4[i][j], father.weight4[i][j]);
        }
        child.bias4[i] = getAve(mother.bias4[i], father.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            child.weight5[i][j] = getAve(mother.weight5[i][j], father.weight5[i][j]);
        }
        child.bias5[i] = getAve(mother.bias5[i], father.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        child.weight6[j] = getAve(mother.weight6[j], father.weight6[j]);
    }
    return child;
}

// 突然変異付きのどっちか選ぶ関数 (float限定)
float alternativeFM(float f1, float f2) {
    // 突然変異!
    if (randFloat() < MUT_RATE) return randWeight();
    // 突然変異しなければ二者択一
    return alternative(f1, f2);
}

// 一様交叉 (突然変異付き)
Param uniformCrossoverM(Param mother, Param father) {
    Param child;
    int i, j;
    // first layer
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 64; j++) {
            child.weight1[i][j] = alternativeFM(mother.weight1[i][j], father.weight1[i][j]);
        }
        child.bias1[i] = alternativeFM(mother.bias1[i], father.bias1[i]);
    } // second layer
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 32; j++) {
            child.weight2[i][j] = alternativeFM(mother.weight2[i][j], father.weight2[i][j]);
        }
        child.bias2[i] = alternativeFM(mother.bias2[i], father.bias2[i]);
    } // third layer
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j++) {
            child.weight3[i][j] = alternativeFM(mother.weight3[i][j], father.weight3[i][j]);
        }
        child.bias3[i] = alternativeFM(mother.bias3[i], father.bias3[i]);
    } // fourth layer
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 8; j++) {
            child.weight4[i][j] = alternativeFM(mother.weight4[i][j], father.weight4[i][j]);
        }
        child.bias4[i] = alternativeFM(mother.bias4[i], father.bias4[i]);
    } // fifth layer
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            child.weight5[i][j] = alternativeFM(mother.weight5[i][j], father.weight5[i][j]);
        }
        child.bias5[i] = alternativeFM(mother.bias5[i], father.bias5[i]);
    } // sixth layer
    for (j = 0; j < 2; j++) {
        child.weight6[j] = alternativeFM(mother.weight6[j], father.weight6[j]);
    }
    return child;
}

// パラメータどうしの距離を測る
// ユークリッド距離
float paramDistance(Param p1, Param p2) {
    float dist = 0.0f;
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
    // 個体数で割り, 平方根を返す
    return sqrtf(dist / PARAM_NUM);
}

// 次の世代へ!
int nextGeneration(int gene_num) {
    int i, j, count;
    char format[] = "generation%03d.bin";
    char fnamer[FILENAME_MAX], fnamew[FILENAME_MAX];
    Param parents[ELITE_NUM];
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
    for (count = 0; count < ELITE_NUM; count++)
        cgene.prms[count] = parents[count];
    // make children
    // 45 combinations, 2 children per couple
    for (i = 0; i < ELITE_NUM - 1; i++) {
        for (j = i + 1; j < ELITE_NUM; j++) {
            cgene.prms[count] = averageCrossover(parents[i], parents[j]);
            count++;
            cgene.prms[count] = uniformCrossoverM(parents[i], parents[j]);
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

// 何だこれ
int readResultFile(int *result, int r_size, const char *fnamel) {
    FILE *fp;
    // read file
    if ((fp = fopen(fnamel, "rb")) == NULL) {
        // 失敗
        printf("\a%s cannot be opened\n", fnamel);
        return -1;
    }
    // 成功
    fread(result, r_size, 1, fp);
    fclose(fp);
    return 0;
}