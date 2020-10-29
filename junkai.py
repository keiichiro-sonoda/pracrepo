# 巡回セールスマン問題を解きたい
# 並び替えの交叉は01のときと異なるのでその練習をしたい
import random as rd
import numpy as np
import matplotlib.pyplot as plt

rd.seed(123)
np.random.seed(123)

# 遺伝子長
# つまり拠点の数
LENGTH = 7

# サンプル個体(0オリジンとした)
FATHER = [2, 4, 1, 3, 6, 5, 0]
MOTHER = [3, 2, 5, 4, 1, 0, 6]

# なんとなくクラスにしてみる
# Traveling Salesman Problem
class TSP():

    # 経路を求めるための座標を与える
    def __init__(self, coordinates):
        self.coordinates = coordinates
        print(self.coordinates)

    # 循環交叉
    # 親を2つ与える
    # 子も2つタプルで返す予定
    def circularCrossing(self, p1, p2):
        # まず子供をコピーしておく
        c1 = p1.copy()
        c2 = p2.copy()
        # ランダムな添え字を一つ選ぶ
        i = rd.randint(0, LENGTH - 1)
        # 最初の値
        st = p1[i]
        # 今見ている値(次?)
        now = p2[i]
        # 固定する添え字のリスト
        fixed = [i]
        # 最初の値に戻るまでくり返し
        while now != st:
            # 親2と同じ値がある親1の添え字を入手
            i = p1.index(now)
            # リストに追加
            fixed.append(i)
            # 次の値を代入
            now = p2[i]
        # 相互の親から引継ぎ
        for i in fixed:
            c1[i] = p2[i]
            c2[i] = p1[i]
        return c1, c2
    
    # 経路を確認する関数を目指したい
    def viewPath(self, path):
        x = self.coordinates[:, 0]
        y = self.coordinates[:, 1]
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.scatter(x, y)
        for i in range(LENGTH):
            ax.plot([x[path[i - 1]], x[path[i]]], [y[path[i - 1]], y[path[i]]], "k-")
        plt.show()

def main():
    arr = np.random.rand(LENGTH, 2)
    tsp = TSP(arr)
    #c = tsp.circularCrossing(FATHER, MOTHER)
    #print(c)
    #print(arr[0])
    # 縦要素の抽出(列?)
    tsp.viewPath(MOTHER)


if __name__ == "__main__":
    main()