import numpy as np
import matplotlib.pyplot as plt

def view2DArrSamp1():
    x = np.arange(0, 10, 0.05)
    y = np.arange(0, 10, 0.05)

    X, Y = np.meshgrid(x, y)
    Z = np.sin(X) + np.cos(Y)

    # 等高線図の生成. cmapで色付けの規定を指定する.
    #plt.pcolormesh(X, Y, Z, cmap='hsv')
    plt.pcolor(X, Y, Z, cmap='hsv')

    pp = plt.colorbar(orientation="vertical")
    pp.set_label("Label", fontname="Arial", fontsize=24)

    plt.xlabel('X', fontsize=24)
    plt.ylabel('Y', fontsize=24)

    plt.show()

def view2DArrSamp2():
    z = np.zeros([100, 100])
    for i in range(100):
        for j in range(100):
            z[i, j] = i + j
    
    plt.imshow(z)
    # カラーバーの表示
    plt.colorbar()
    plt.xlabel('X')
    plt.ylabel('Y')
    plt.show()

if __name__ == "__main__":
    view2DArrSamp2()