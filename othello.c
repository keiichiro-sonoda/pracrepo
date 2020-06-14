#include <stdio.h>
#include <math.h>
#include <time.h>
#include <unistd.h>

// maximum number of next boards
#define NEXT_MAX 32
// 1 million
#define MILLION 1000000
// 1 billion
#define BILLION 1000000000

#define START_H 0x0000000000000180L
#define START_L 0x0240000000000000L

#define kugiri() printf("--------------------------\n")

// 64bit
typedef unsigned long int int8B;

// othello board
// 128 bits
typedef struct {
    // low bits, high bits
    int8B board[2];
} Board;

// intial board
Board START;

// bin -> char
// black: (0b01, o), white: (0b10, x), empty: (0b00, -)
// can put sign: (0b11, !)
const char B2C[5] = "-ox!";

// 8 directions
const int DIRECTION[8] = {18, 16, 14, 2, -2, -14, -16, -18};

// get a piece at a certain address
int getKoma(Board b, int ad) {
    return (b.board[ad >> 6] >> (ad & 0x3f)) & 0b11;
}

// put a piece at a certain address
void putKoma(Board *bp, int ad, int koma) {
    bp->board[ad >> 6] |= (int8B)koma << (ad & 0x3f);
    return;
}

// empty the board
void emptyBoard(Board *bp) {
    bp->board[0] = 0;
    bp->board[1] = 0;
    return;
}

Board createEmptyBoard(void) {
    Board eb;
    emptyBoard(&eb);
    return eb;
}

// show a board
int showBoard(Board b) {
    // column symbol
    char str[163] = "  A B C D E F G H\n";
    int i, j, index;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 16; j += 2) {
            index = 160 - j - i * 18;
            str[index] = B2C[getKoma(b, i * 16 + j)];
            str[index - 1] = ' ';
        }
        str[161 - 18 * i] = '\n';
        // row symbol
        str[144 - 18 * i] = '8' - i;
    }
    // null
    str[162] = '\0';
    // display
    printf("%s", str);
    return 0;
}

// show board in 32 hexadecimal digits
int showBoardHex(Board b) {
    printf("%016lx%016lx\n", b.board[1], b.board[0]);
    return 0;
}

int showDecimalArray(int *ia, int ia_len) {
    int i;
    for (i = 0; i < ia_len; i++) {
        printf("%03d ", ia[i]);
    }
    putchar('\n');
    return 0;
}

int showHexArray(int *ia, int ia_len) {
    int i;
    for (i = 0; i < ia_len; i++) {
        printf("%02x ", ia[i]);
    }
    putchar('\n');
    return 0;
}

// neighbor?
int ifNeighbor(int src, int dst) {
    // range over
    if (dst < 0 || 126 < dst) return 0;
    int c1, c2;
    c1 = src % 16;
    c2 = dst % 16;
    // left or right over
    if ((c1 == 0 && c2 == 14) || (c1 == 14 && c2 == 0)) return 0;
    // neighbor!!
    return 1;
}

// check if the element is in the array
int inArray(int *ar, int ar_len, int el) {
    int i;
    // check all elements
    for (i = 0; i < ar_len; i++) {
        if (ar[i] == el) return 1;
    }
    return 0;
}

int getIndex(const int *ar, int ar_len, int el) {
    int i;
    // check all elements
    for (i = 0; i < ar_len; i++) {
        if (ar[i] == el) return i;
    }
    // given element does not exist
    return -1;
}

// get maximum value of array
int getMax(int *ar, int ar_len) {
    int i;
    int mx = -BILLION;
    for (i = 0; i < ar_len; i++) {
        mx = (mx < ar[i] ? ar[i] : mx);
    }
    return mx;
}

// reverse a piece at a certain address
// give a Board pointer to rewrite it
void reOneAd(Board *bp, int ad) {
    (bp->board)[ad >> 6] ^= (int8B)0b11 << (ad & 0x3F);
    return;
}

// reverse pieces in some addresses?
void reRange(Board *bp, int *ads, int length) {
    int i;
    for (i = 0; i < length; i++)
        reOneAd(bp, ads[i]);
    return;
}

// find addresses to put black
// and calculate nextboard
int canPutBlackPlus(Board b, int *cpb, Board *nbs) {
    int i, j, ad, dir, koma;
    int index = 0;
    int flag = 0;
    // reversible addresses
    int ads[8];
    // check all addresses
    for (ad = 0; ad < 128; ad += 2) {
        // not empty
        if (getKoma(b, ad)) continue;
        // empty
        for (i = 0; i < 8; i++) {
            dir = DIRECTION[i];
            ads[0] = ad + dir;
            // neighbor!
            if (ifNeighbor(ad, ads[0])) {
                // not white
                if (getKoma(b, ads[0]) ^ 0b10) continue;
                // white
                ads[1] = ads[0] + dir;
                // while -> for
                for (j = 1; ifNeighbor(ads[j - 1], ads[j]); j++) {
                // check the diagonal
                    koma = getKoma(b, ads[j]);
                    // black
                    // can put!!
                    if (koma == 0b01) {
                        // first
                        if (!flag) {
                            // copy board
                            nbs[index] = b;
                            cpb[index] = ad;
                            flag = 1;
                        }
                        // reverse
                        reRange(nbs + index, ads, j);
                        break;
                    } // empty
                    else if (koma == 0b00) break;
                    // white
                    // may be able to reverse
                    ads[j + 1] = ads[j] + dir;
                }

            }
        }
        // all directions were checked and reversed
        // put the piece!
        if (flag) {
            putKoma(nbs + index, ad, 0x01);
            index++;
            flag = 0;
        }
    }
    // return array length
    return index;
}

// return next board (put black)
// if invalid action, return same board
Board nextBoardBlack(Board b, int te) {
    // not empty
    if (getKoma(b, te)) return b;
    int i, j, con, dir;
    int ads[8];
    // validity
    int valid = 0;
    Board nb;
    // deep copy
    nb = b;
    // check 8 directions
    for (i = 0; i < 8; i++) {
        dir = DIRECTION[i];
        ads[0] = te + dir;
        // not neighbor
        if (!ifNeighbor(te, ads[0])) continue;
        // not white
        if (getKoma(b, ads[0]) ^ 0b10) continue;
        ads[1] = ads[0] + dir;
        // continue flag
        con = 1;
        // check diagonal
        for (j = 1; con && ifNeighbor(ads[j - 1], ads[j]); j++) {
            switch (getKoma(b, ads[j])) {
                // white
                case 0b10:
                    ads[j + 1] = ads[j] + dir;
                    break;
                // black
                case 0b01:
                    reRange(&nb, ads, j);
                    valid = 1;
                // black or empty
                default:
                    con = 0;
            }
        }
    }
    // can put!!
    if (valid) {
        putKoma(&nb, te, 0b01);
    } else printf("cannot put\n");
    return nb;
}

// return next board (put white)
Board nextBoardWhite(Board b, int te) {
    // not empty
    if (getKoma(b, te)) return b;
    int i, j, dir, koma;
    int ads[8];
    // changed
    int flag = 0;
    // check 8 directions
    for (i = 0; i < 8; i++) {
        dir = DIRECTION[i];
        ads[0] = te + dir;
        // not neighbor or neighbor is not black
        if (!ifNeighbor(te, ads[0]) || (getKoma(b, ads[0]) ^ 0b01)) continue;
        ads[1] = ads[0] + dir;
        // check the diagonal
        for (j = 1; ifNeighbor(ads[j - 1], ads[j]); j++) {
            koma = getKoma(b, ads[j]);
            // black
            if (koma == 0b01) {
                ads[j + 1] = ads[j] + dir;
                continue;
            } // white
            else if (koma == 0b10) {
                reRange(&b, ads, j);
                flag = 1;
            }
            // white or empty
            break;
        }
    }
    // put white
    if (flag) {
        putKoma(&b, te, 0b10);
    }
    return b;
}

// find addresses to put white (black base)
// and calculate nextboard
int canPutWhitePlus(Board b, int *cpw, Board *nbs) {
    int i, j, ad, oad, dir, koma;
    int index = 0;
    int ads[8];
    // temporary variable
    Board cb;
    // check all addresses
    for (ad = 0; ad < 128; ad += 2) {
        // not black
        if (getKoma(b, ad) ^ 0b01) continue;
        // check 8 directions
        for (i = 0; i < 8; i++) {
            dir = DIRECTION[i];
            // check the opposite
            oad = ad - dir;
            // not neighbor or neighbor is not empty
            if (!ifNeighbor(ad, oad) || getKoma(b, oad)) continue;
            // already known
            if (inArray(cpw, index, oad)) continue;
            ads[0] = ad;
            ads[1] = ad + dir;
            // check the diagonal
            for (j = 1; ifNeighbor(ads[j - 1], ads[j]); j++) {
                koma = getKoma(b, ads[j]);
                // black
                if (koma == 0b01) {
                    ads[j + 1] = ads[j] + dir;
                    continue;
                } // white
                if (koma == 0b10) {
                    cpw[index] = oad;
                    // copy
                    cb = b;
                    reRange(&cb, ads, j);
                    nbs[index] = nextBoardWhite(cb, oad);
                    // just in case
                    putKoma(nbs + index, oad, 0b10);
                    index++;
                }
                // white or empty
                break;
            }
        }
    }
    return index;
}

// show some boards
void showBoardArray(const Board *ba, int ba_len) {
    int i;
    for (i = 0; i < ba_len; i++) {
        showBoardHex(ba[i]);
        showBoard(ba[i]);
    }
    return;
}

// swap white and black
Board swapBoard(Board b) {
    Board sb;
    int i;
    sb.board[0] = 0;
    sb.board[1] = 0;
    for (i = 0; i < 64; i++) {
        sb.board[0] = (sb.board[0] << 1) | (b.board[1] & 0b1);
        sb.board[1] = (sb.board[1] << 1) | (b.board[0] & 0b1);
        b.board[1] >>= 1;
        b.board[0] >>= 1;
    }
    return sb;
}

// get action from standard input
int getActStdin(void) {
    int i, ad, con;
    int flag = 1;
    char abc;
    // redo if invalid string
    while (flag) {
        con = 1;
        printf("input action: ");
        for (i = 0; (abc = getchar()) != '\n'; i++) {
            if (i == 0 && 'A' <= abc && abc <= 'H') {
                ad = ('H' - abc) << 1;
            } else if (con && i == 1 && '1' <= abc && abc <= '8') {
                ad += ('8' - abc) << 4;
                flag = 0;
            } else con = 0;
        }
    }
    //printf("address: %d\n", ad);
    return ad;
}

int getValidActStdin(int *can_put, int length) {
    if (length == 0) return -1;
    int te;
    while (1) {
        te = getActStdin();
        if (inArray(can_put, length, te)) break;
        printf("invalid action\n");
    }
    return te;
}

int ad2coo(int ad, char *dst) {
    if (ad < 0 || 126 < ad) return -1;
    dst[0] = 'H' - (ad & 0xf) / 2;
    dst[1] = '8' - (ad >> 4);
    dst[2] = '\0';
    return 0;
}

int showCoordinates(const int *can_put, int length) {
    if (length == 0) {
        printf("pass\n");
        return -1;
    }
    int i;
    char str[3];
    printf("available: ");
    for (i = 0; i < length; i++) {
        ad2coo(can_put[i], str);
        printf("%s ", str);
    }
    putchar('\n');
    return 0;
}

int showCanPut(Board b, const int *can_put, int next_count) {
    int i;
    for (i = 0; i < next_count; i++)
        putKoma(&b, can_put[i], 0b11);
    showBoard(b);
    showCoordinates(can_put, next_count);
    return 0;
}

int showCanPutPlus(Board b, int color, int *can_put, Board *next_boards) {
    int i, length;
    switch (color) {
        // black
        case 0b01:
            length = canPutBlackPlus(b, can_put, next_boards);
            break;
        // white
        case 0b10:
            length = canPutWhitePlus(b, can_put, next_boards);
            break;
        default:
            length = 0;
    }
    for (i = 0; i < length; i++) {
        putKoma(&b, can_put[i], 0b11);
    }
    // view
    showBoard(b);
    showCoordinates(can_put, length);
    return length;
}


// all zero
int zeros(int *ia, int ia_len) {
    int i;
    for (i = 0; i < ia_len; i++) {
        ia[i] = 0;
    }
    return 0;
}

// all zero 2D array
// int iaa[l1][l2]
int zeros2D(int *iaa[], int l1, int l2) {
    int i;
    for (i = 0; i < l1; i++) {
        zeros(iaa[i], l2);
    }
    return 0;
}

int indexes(int *ia, int ia_len) {
    int i;
    for (i = 0; i < ia_len; i++)
        ia[i] = i;
    return 0;
}

// 5 arguments
// count the number of pieces
int canPutPP(Board b, int color, int *can_put, Board *next_boards, int *koma_count) {
    int i, j, ad, dir, koma;
    int index = 0;
    int flag = 0;
    // reversible addresses
    int ads[8];
    // opponent's color
    int opc = color ^ 0b11;
    // reset count
    zeros(koma_count, 3);
    // check all addresses
    for (ad = 0; ad < 128; ad += 2) {
        // check a piece
        koma = getKoma(b, ad);
        koma_count[koma]++;
        // not empty
        if (koma) continue;
        // empty
        for (i = 0; i < 8; i++) {
            dir = DIRECTION[i];
            ads[0] = ad + dir;
            // not neighbor or the neighbor is not opponent
            if (!ifNeighbor(ad, ads[0]) || (getKoma(b, ads[0]) ^ opc)) continue;
            // white
            ads[1] = ads[0] + dir;
            // check the diagonal
            for (j = 1; ifNeighbor(ads[j - 1], ads[j]); j++) {
                koma = getKoma(b, ads[j]);
                // my color
                // can put!!
                if (koma == color) {
                    // first
                    if (!flag) {
                        // copy board
                        next_boards[index] = b;
                        can_put[index] = ad;
                        flag = 1;
                    }
                    // reverse
                    reRange(next_boards + index, ads, j);
                    break;
                } // empty
                else if (koma == 0b00) break;
                // opponent's color
                ads[j + 1] = ads[j] + dir;
            }
        }
        // all directions were checked and reversed
        // put the piece!
        if (flag) {
            putKoma(next_boards + index, ad, color);
            index++;
            flag = 0;
        }
    }
    // return array length
    return index;
}

int showCanPutPP(Board b, int color, int *can_put, Board *next_boards) {
    int i, length;
    int kc[3] = {0, 0, 0};
    length = canPutPP(b, color, can_put, next_boards, kc);
    for (i = 0; i < length; i++) {
        putKoma(&b, can_put[i], 0b11);
    }
    // view
    showBoard(b);
    showCoordinates(can_put, length);
    return length;
}

// nega max
int negaMax(Board b, int color, int depth, int pass) {
    int i, point, tp, nc, dif;
    int cpa[NEXT_MAX];
    int kc[3] = {0, 0, 0};
    int opc = color ^ 0b11;
    Board nba[NEXT_MAX];
    // calculate next
    nc = canPutPP(b, color, cpa, nba, kc);
    // leaf
    if (nc == 0 && pass) {
        dif = kc[color] - kc[opc];
        point = dif * MILLION;
    } // max
    else if (depth <= 0) {
        dif = kc[color] - kc[opc];
        point = nc * kc[0] + dif * (64 - kc[0]);
        //printf("%d\n", point);
    } // pass
    else if (nc == 0) {
        point = negaMax(b, opc, depth - 1, 1);
    } // general
    else {
        point = -BILLION;
        // search next
        for (i = 0; i < nc; i++) {
            tp = negaMax(nba[i], opc, depth - 1, 0);
            // get the bigger one
            point = (point < tp ? tp : point);
        }
    }
    return -point;
}

int wrapNegaMax(Board b, int color) {
    // best action?
    int best_te = -1;
    int index, nc, pt, te;
    int cpa[NEXT_MAX];
    int kc[3] = {0, 0, 0};
    int opc = color ^ 0b11;
    int mxpt = -BILLION;
    Board nba[NEXT_MAX];
    char moji[3];
    // calculate next
    nc = canPutPP(b, color, cpa, nba, kc);
    // cannot put
    if (nc == 0) return -1;
    // opponent's turn
    for (index = 0; index < nc; index++) {
        pt = negaMax(nba[index], opc, 4, 0);
        te = cpa[index];
        // bad action??
        if (te == 18 || te == 28 || te == 98 || te == 108) {
            pt -= 500;
        // corner
        } else if (te == 0 || te == 16 || te == 112 || te == 126) {
            pt += 500;
        }
        // update maximum value
        if (mxpt < pt) {
            mxpt = pt;
            // update the best action
            best_te = cpa[index];
        }
    }
    ad2coo(best_te, moji);
    printf("best action?: %s, point: %d\n", moji, mxpt);
    return best_te;
}

// alpha beta
int negaMaxAB(Board b, int color, int depth, int pass, int alpha, int beta) {
    int i, tp, nc, dif;
    int cpa[NEXT_MAX];
    int kc[3] = {0, 0, 0};
    int opc = color ^ 0b11;
    Board nba[NEXT_MAX];
    // calculate next
    nc = canPutPP(b, color, cpa, nba, kc);
    // leaf
    if (nc == 0 && pass) {
        dif = kc[color] - kc[opc];
        alpha = dif * MILLION;
    } // finish
    else if (depth <= 0) {
        dif = kc[color] - kc[opc];
        alpha = nc * kc[0] + dif * (64 - kc[0]);
    } // pass
    else if (nc == 0) {
        alpha = negaMaxAB(b, opc, depth - 1, 1, -beta, -alpha);
    } // general
    else {
        // search next
        for (i = 0; i < nc; i++) {
            tp = negaMaxAB(nba[i], opc, depth - 1, 0, -beta, -alpha);
            // get the bigger one
            alpha = (alpha < tp ? tp : alpha);
            if (alpha >= beta) break;
        }
    }
    return -alpha;
}

int wrapNegaMaxAB(Board b, int color, int height) {
    // best action?
    int best_te = -1;
    int index, nc, pt, te;
    int cpa[NEXT_MAX];
    int kc[3] = {0, 0, 0};
    int opc = color ^ 0b11;
    int alpha = -BILLION;
    Board nba[NEXT_MAX];
    char moji[3];
    // calculate next
    nc = canPutPP(b, color, cpa, nba, kc);
    // cannot put
    if (nc == 0) return -1;
    // opponent's turn
    for (index = 0; index < nc; index++) {
        pt = negaMaxAB(nba[index], opc, 4, 0, -BILLION, -alpha);
        te = cpa[index];
        // bad action??
        if (te == 18 || te == 28 || te == 98 || te == 108) {
            pt -= 500;
        // corner
        } else if (te == 0 || te == 16 || te == 112 || te == 126) {
            pt += 500;
        }
        // update maximum value
        if (alpha < pt) {
            alpha = pt;
            // update the best action
            best_te = cpa[index];
        }
    }
    // show result
    ad2coo(best_te, moji);
    printf("best action?: %s, point: %d\n", moji, alpha);
    return best_te;
}

int play(void) {
    Board main_board = START;
    int count, te, index;
    int t_count = 1;
    int flag = 0;
    int can_put[NEXT_MAX];
    int kc[3] = {0, 0, 0};
    Board next_boards[NEXT_MAX];
    // initial turn (black)
    int turn = 0b01;
    while (1) {
        printf("turn %2d: %c\n", t_count, B2C[turn]);
        count = showCanPutPP(main_board, turn, can_put, next_boards);
        // cannot put a piece anywhere
        if (count == 0) {
            // cannot do anything each other
            if (flag) {
                printf("end\n");
                break;
            }
            // pass
            turn ^= 0b11;
            flag = 1;
            continue;
        }
        // continue
        flag = 0;
        t_count++;
        // black (stdin)
        if (turn == 0b01) {
            te = wrapNegaMaxAB(main_board, turn, 6);
            //te = wrapNegaMax(main_board, turn);
            //te = getValidActStdin(can_put, count);
            index = getIndex(can_put, count, te);
        } // white (auto)
        else {
            te = wrapNegaMaxAB(main_board, turn, 4);
            //te = wrapNegaMax(main_board, turn);
            index = getIndex(can_put, count, te);
        } // update board
        main_board = next_boards[index];
        // next turn
        turn ^= 0b11;
    }
    return 0;
}

int rotL90DegAd(int src) {
    return (14 - src % 16) * 8 + (src / 16) * 2;
}

// rotate the board 90 degrees to the left
Board rotL90DegBoard(Board b1) {
    Board b2 = createEmptyBoard();
    int src;
    for (src = 0; src < 128; src += 2)
        putKoma(&b2, rotL90DegAd(src), getKoma(b1, src));
    return b2;
}

Board mirrorHLBoard(Board b1) {
    Board b2 = createEmptyBoard();
    int i, j, row;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            row = (b1.board[i] >> 16 * j) & 0xFFFF;
            b2.board[1 - i] |= (int8B)row << 16 * (3 - j);
        }
    }
    return b2;
}

// get the smaller board
Board minBoard(Board b1, Board b2) {
    if (b1.board[1] < b2.board[1])
        return b1;
    if (b1.board[1] > b2.board[1])
        return b2;
    if (b1.board[0] < b2.board[0])
        return b1;
    if (b1.board[0] > b2.board[0])
        return b2;
    return b1;
}

// normalize a board
Board normalBoard(Board b1) {
    Board b2, b3, b4, b5, b6, b7, b8, bm;
    b2 = rotL90DegBoard(b1);
    bm = minBoard(b1, b2);
    b3 = rotL90DegBoard(b2);
    bm = minBoard(bm, b3);
    b4 = rotL90DegBoard(b3);
    bm = minBoard(bm, b4);
    b5 = mirrorHLBoard(b1);
    bm = minBoard(bm, b5);
    b6 = mirrorHLBoard(b2);
    bm = minBoard(bm, b6);
    b7 = mirrorHLBoard(b3);
    bm = minBoard(bm, b7);
    b8 = mirrorHLBoard(b4);
    bm = minBoard(bm, b8);
    return bm;
}

// initial configure
void initBoard(void) {
    START.board[1] = START_H;
    START.board[0] = START_L;
    return;
}

void normalizeBoard(Board *bp) {
    *bp = normalBoard(*bp);
    return;
}

// swap and normalize a board
void swapNormalizeBoard(Board *bp) {
    *bp = normalBoard(swapBoard(*bp));
    return;
}

int sameBoard(Board b1, Board b2) {
    if (b1.board[0] != b2.board[0]) return 0;
    if (b1.board[1] != b2.board[1]) return 0;
    return 1;
}

// check if it's a known board
int knownBoard(const Board *ba, int n, Board b) {
    int i;
    for (i = 0; i < n; i++) {
        if (sameBoard(ba[i], b)) return 1;
    }
    return 0;
}

// give a normalized board
// next turn is always black
int nextBoardNormal(Board b, Board *next_boards) {
    int index = 0;
    int i, j, ad, dif, cnt;
    int ads[8];
    Board nb_t[0];
    int flag = 0;
    // search all addresses
    for (ad = 0; ad < 128; ad += 2) {
        // not empty
        if (getKoma(b, ad)) continue;
        // search neighbor
        for (i = 0; i < 8; i++) {
            dif = DIRECTION[i];
            ads[0] = ad + dif;
            // not neighbor or neighbor is not white
            if (!ifNeighbor(ad, ads[0]) || getKoma(b, ads[0]) ^ 0b10) continue;
            ads[1] = ads[0] + dif;
            cnt = 1;
            // search the diagonal
            for (j = 1; cnt && ifNeighbor(ads[j - 1], ads[j]); j++) {
                switch(getKoma(b, ads[j])) {
                    // white
                    case 0b10:
                        ads[j + 1] = ads[j] + dif;
                        continue;
                    // black
                    case 0b01:
                        // first
                        if (!flag) {
                            // board copy
                            nb_t[0] = b;
                            flag = 1;
                        }
                        // reverse!
                        reRange(nb_t, ads, j);
                    // black or empty
                    default:
                        cnt = 0;
                        break;
                }
            }
        }
        // can put
        if (flag) {
            // put black
            putKoma(nb_t, ad, 0b01);
            swapNormalizeBoard(nb_t);
            // new board
            if (!knownBoard(next_boards, index, nb_t[0])) {
                next_boards[index] = nb_t[0];
                index++;
            }
            flag = 0;
        }
    }
    // check
    showBoardArray(next_boards, index);
    return index;
}

// count the number of pieces
int nextBoardNormal2(Board b, Board *next_boards, int *koma_count) {
    int i, j, ad, dir, koma;
    int index = 0;
    int flag = 0;
    // reversible addresses
    int ads[8];
    // reset count
    zeros(koma_count, 3);
    // swap the board
    Board sb = swapBoard(b);
    // check all addresses
    for (ad = 0; ad < 128; ad += 2) {
        // check a piece
        koma = getKoma(sb, ad);
        koma_count[koma]++;
        // not empty
        if (koma) continue;
        // empty
        for (i = 0; i < 8; i++) {
            dir = DIRECTION[i];
            ads[0] = ad + dir;
            // not neighbor or the neighbor is not black
            if (!ifNeighbor(ad, ads[0]) || (getKoma(sb, ads[0]) ^ 0b01)) continue;
            // white
            ads[1] = ads[0] + dir;
            // check the diagonal
            for (j = 1; ifNeighbor(ads[j - 1], ads[j]); j++) {
                koma = getKoma(sb, ads[j]);
                // white
                // can put!!
                if (koma == 0b10) {
                    // first
                    if (!flag) {
                        // copy the swapped board
                        next_boards[index] = sb;
                        flag = 1;
                    }
                    // reverse
                    reRange(next_boards + index, ads, j);
                    break;
                } // empty
                else if (koma == 0b00) break;
                // black
                ads[j + 1] = ads[j] + dir;
            }
        }
        // all directions were checked and reversed
        if (flag) {
            // put white
            putKoma(next_boards + index, ad, 0b10);
            normalizeBoard(next_boards + index);
            // new board
            if (!knownBoard(next_boards, index, next_boards[index]))
                index++;
            flag = 0;
        }
    }
    // return array length
    return index;
}

// main
int main2(void) {
    initBoard();
    // sample boards
    Board sample1, sample2;
    Board nbs[32];
    int i;
    int kc[3];
    sample1.board[0] = 0xaaaa2aa902aa5541;
    sample1.board[1] = 0x00000000000000aa;
    sample2.board[0] = 0xaaaa28a90aaa5545;
    sample2.board[1] = 0x0000200209021202;
    //play();
    showBoard(sample2);
    i = nextBoardNormal2(sample2, nbs, kc);
    showBoardArray(nbs, i);
    return 0;
}
