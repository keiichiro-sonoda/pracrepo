# 巡回セールスマン問題を解きたい
# 並び替えの交叉は01のときと異なるのでその練習をしたい
import random as rd

# 遺伝子長
# つまり拠点の数
LENGTH = 6

father = [2, 4, 1, 3, 6, 5]
mother = [3, 2, 5, 4, 1, 6]

# 親を2つ与える
def circularCrossing(p1, p2):
    # ランダムな添え字を一つ選ぶ
    r = rd.randint(0, LENGTH - 1)
    # 添え字とそれぞれの値を確認
    print(r, p1[r], p2[r])

def main():
    circularCrossing(father, mother)

if __name__ == "__main__":
    main()