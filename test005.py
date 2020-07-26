# 突然変異除いたバージョンのパラメータのグラフを作りたい
from ctypes import *

# 共有ライブラリ読み込み(同じディレクトリで実行)
exe2_win = cdll.LoadLibrary("exe2_win.so")
# 配列型定義
FloatArray64 = c_float * 64
FloatArray10 = c_float * 10

# 初期化関数を実行
exe2_win.initPy()
# 関数取り出し
printString = exe2_win.printStringPy
printString.rectype = None
printString.argtypes = (c_char_p, )

if __name__ == "__main__":
    byte_moji = "sprm_corner0.5_{:03d}.bin".format(50).encode()
    printString(byte_moji)