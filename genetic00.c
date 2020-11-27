// パラメータチェック
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
