#include <stdio.h>

// functions defined in othello.c
int rotL90DegAd(int src);
void showDecimalArray(const int *ia, int ia_len);

int getMin(int a, int b) {
    return (a < b ? a : b);
}

// normalize an address
int normalAd(int ad1) {
    int ad2, ad3, ad4, adm;
    ad2 = rotL90DegAd(ad1);
    adm = getMin(ad1, ad2);
    ad3 = rotL90DegAd(ad2);
    adm = getMin(adm, ad3);
    ad4 = rotL90DegAd(ad3);
    return getMin(adm, ad4);
}

int main(void) {
    printf("Hello World!!\n");
    int ad2index[64];
    int i, ad, eq_ad;
    for (i = 0; i < 64; i++) {
        ad = i * 2;
        eq_ad = normalAd(ad);
        ad2index[i] = eq_ad;
    }
    showDecimalArray(ad2index, 64);
    return 0;
}