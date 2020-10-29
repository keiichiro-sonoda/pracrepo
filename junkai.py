# 巡回セールスマン問題を解きたい
# 並び替えの交叉は01のときと異なるのでその練習をしたい
# 円順列, じゅず順列等は考えずに等価な個体も別物として扱う
import random as rd
import numpy as np
import matplotlib.pyplot as plt
import json

rd.seed(123)
np.random.seed(123)

# 遺伝子長
# つまり拠点の数
LENGTH = 3

# Traveling Salesman Problem
class TSP():

    # 経路を求めるための座標を与える
    # numpy 配列を与える
    def __init__(self, coordinates):
        self.coordinates = coordinates
        #print(self.coordinates)
        path = self.makeRandomPath()
        #self.viewPath(path)
        self.makeDistTable()

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
    
    # ランダムな経路を作成する関数
    def makeRandomPath(self):
        return rd.sample(range(LENGTH), LENGTH)
    
    # 2点間の距離を計算する
    def calcDist(self, a, b):
        return np.sqrt(2)
    
    # 各拠点間の距離を保存する表を作りたい
    # 拠点数 × 拠点数の2次元配列
    def makeDistTable(self):
        # 全て0で初期化
        self.dist_table = np.zeros((LENGTH, LENGTH))
        for i in range(LENGTH):
            for j in range(LENGTH):
                # 同じ拠点同士は除く
                if i != j:
                    self.dist_table[i, j] = self.calcDist(i, j)
        print(self.dist_table)
    
    # ただ座標を確認する関数
    def viewCoordinates(self):
        x = self.coordinates[:, 0]
        y = self.coordinates[:, 1]
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.scatter(x, y)
        plt.show()
    
    # 経路を確認する関数
    def viewPath(self, path):
        x = self.coordinates[:, 0]
        y = self.coordinates[:, 1]
        fig = plt.figure()
        ax = fig.add_subplot(111)
        ax.scatter(x, y)
        # 添え字-1が末尾を示す性質を利用
        for i in range(LENGTH):
            ax.plot([x[path[i - 1]], x[path[i]]], [y[path[i - 1]], y[path[i]]], "k-")
        plt.show()
    
    # 経路情報を記録したファイルを作りたい
    # json形式
    def makeSampleFile(self):
        # シード固定
        np.random.seed(123)
        s_arr = np.random.rand(LENGTH, 2)
        s_list = s_arr.tolist()
        print(s_list)
        s_fname = "C:\\Users\\17T2088B\\GitHub\\pracrepo\\dat\\coord100_samp01.json"
        f = open(s_fname, "w")
        json.dump(s_list, f)
        f.close()

def main():
    fname = "C:\\Users\\17T2088B\\GitHub\\pracrepo\\dat\\coord100_samp01.json"
    f = open(fname, "r")
    l = json.load(f)
    f.close()
    #arr = np.array(l)
    arr = np.random.rand(LENGTH, 2)
    tsp = TSP(arr)
    #tsp.viewCoordinates()

if __name__ == "__main__":
    main()