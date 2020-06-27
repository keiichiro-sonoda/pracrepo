# -*- coding: utf-8 -*-
# ctypes の練習
import numpy as np
import sys
import ctypes

kernel32 = ctypes.WinDLL("kernel32")
kernel32.Sleep.restype = None # void
kernel32.Sleep.argtypes = (ctypes.c_uint32,) # (unsigned int)

print("started")
sys.stdout.flush()
kernel32.Sleep(1000)
print("finished")