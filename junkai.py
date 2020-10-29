# 巡回セールスマン問題を解きたい
# 並び替えの交叉は01のときと異なるのでその練習をしたい
import random as rd

rd.seed(1)

# 遺伝子長
# つまり拠点の数
LENGTH = 6

father = [2, 4, 1, 3, 6, 5]
mother = [3, 2, 5, 4, 1, 6]

# 循環交叉
# 親を2つ与える
def circularCrossing(p1, p2):
    c1 = p1.copy
    c2 = p2.copy
    # ランダムな添え字を一つ選ぶ
    r = rd.randint(0, LENGTH - 1)
    # 添え字とそれぞれの値を確認
    print(r, p1[r], p2[r])
    # p1のrにある要素がp2のどこにあるか確認
    # 逆も同様
    print(p1.index(p2[r]))
    print(p2.index(p1[r]))

def main():
    circularCrossing(father, mother)

if __name__ == "__main__":
    main()