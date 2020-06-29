from ctypes import *

exe2_win = cdll.LoadLibrary("exe2_win.so")
FloatArray64 = c_float * 64
exe2_win.initPy()
# ただ確認するだけの関数
checkSprmFile = exe2_win.checkSprmFile
checkSprmFile.rectype = None
checkSprmFile.argtypes = (c_int32,)
#checkSprmFile(600)

getSprmFile = exe2_win.getSprmFilePy
getSprmFile.rectype = None
getSprmFile.argtypes = (c_int32, FloatArray64)

def getSprmFileWrap(n):
    f_arr_c = FloatArray64()
    getSprmFile(n, f_arr_c)
    return list(f_arr_c)

if __name__ == "__main__":
    l1 = getSprmFileWrap(300)
    print(l1)