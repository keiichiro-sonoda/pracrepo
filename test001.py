# -*- coding: utf-8 -*-
# ctypes の練習
import numpy as np
import sys
import ctypes

# kernel32とかいうファイルを読み込む?
kernel32 = ctypes.WinDLL("kernel32")
# 返り値指定
kernel32.Sleep.restype = None # void
# 引数指定(要素1の配列)
# unit と間違えないように! (間違えた)
kernel32.Sleep.argtypes = (ctypes.c_uint32,) # (unsigned int)

print("started")
sys.stdout.flush()
kernel32.Sleep(1000)
print("finished")