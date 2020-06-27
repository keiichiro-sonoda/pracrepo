# -*- coding: utf-8 -*-
# ctypes の練習
import numpy as np
import sys
import ctypes

user32 = ctypes.WinDLL("user32")
# A/Wの区別がある場合はWを使う??
user32.MessageBoxW.restype = ctypes.c_int32
user32.MessageBoxW.argtypes = (
    ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint32
)

user32.MessageBoxW(None, u"テスト", u"タイトル", 0)