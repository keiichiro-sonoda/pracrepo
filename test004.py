from ctypes import *

exe2_win = cdll.LoadLibrary("exe2_win.so")
FloatArray64 = c_float * 64
exe2_win.initPy()
checkSprmFile = exe2_win.checkSprmFile
checkSprmFile.rectype = None
checkSprmFile.argtypes = (c_int32,)
checkSprmFile(600)