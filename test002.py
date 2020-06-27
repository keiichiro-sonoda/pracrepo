import ctypes as ct

# 作った共有ライブラリを使う?
libc = ct.cdll.LoadLibrary("./win002.so")
# 関数
add = libc.add
# 返り値を32ビット整数に指定
add.rectype = ct.c_int32
# 引数を32ビット整数2つに指定
add.argtypes = (ct.c_int32, ct.c_int32)

if __name__ == "__main__":
    # 使ってみる!
    print(add(1, 2))