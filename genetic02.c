#include <stdio.h>

// functions defined in othello.c
int rotL90DegAd(int src);
void showDecimalArray(const int *ia, int ia_len);

// functions

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

// ad: normalized address
int ad2index(int ad) {
    if (ad < 16)
        return ad / 2;
    if (ad < 32)
        return ad / 2 - 2;
    if (ad < 48)
        return ad / 2 - 5;
    if (ad < 64)
        return ad / 2 - 11;
    return -1;
}

int main(void) {
    printf("Hello World!!\n");
    int n = 64;
    int indexes[n];
    int i, ad, eq_ad;
    for (i = 0; i < n; i++) {
        ad = i * 2;
        eq_ad = normalAd(ad);
        indexes[i] = eq_ad;
    }
    showDecimalArray(indexes, n);
    for (i = 0; i < n; i++) {
        indexes[i] = ad2index(indexes[i]);
    }
    showDecimalArray(indexes, n);
    return 0;
}