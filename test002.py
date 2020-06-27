import ctypes as ct

# このファイルをメイン関数だと思ってやった方がいいのかも
# 作った共有ライブラリを使う?
libc = ct.cdll.LoadLibrary("./win002.so")
exe2_win = ct.cdll.LoadLibrary("./exe2_win.so")
# 関数
add = libc.add
# 返り値を32ビット整数に指定
add.rectype = ct.c_int32
# 引数を32ビット整数2つに指定
add.argtypes = (ct.c_int32, ct.c_int32)

# float 64の型を作る!
# こんなやり方があんのか...
FloatArray64 = ct.c_float * 64

# 大域変数セット
exe2_win.setIndexes()

checkSprmFile = exe2_win.checkSprmFile
checkSprmFile.rectype = None
checkSprmFile.argtypes =(ct.c_int32,)

getSprmFile = exe2_win.getSprmFilePy
# 返り値はなし
getSprmFile.rectype = None
# 引数に float のポインタを入れて, そこに入れてもらおう
# Python ならメモリ確保動的にやってくれる..よね?
getSprmFile.argtypes = (ct.c_int32, FloatArray64)

f_arr_c = FloatArray64()

if __name__ == "__main__":
    # 使ってみる!
    a = add(1, 2)
    print(a, type(a))
    #print(FloatArray64)
    #print(f_arr)
    #checkSprmFile(300)
    getSprmFile(300, f_arr_c)
    # リスト型に変換
    f_arr = list(f_arr_c)
    print(("{:6.4f} " * 64).format(*f_arr))
    print(type(f_arr[0]))
