# -*- coding: utf-8 -*-
# ctypes の練習
import numpy as np
import sys
import ctypes

# 構造体を定義?
class POINT(ctypes.Structure):
    # ただ変数をひとつ作っておく
    # _fields_ という名前も重要?
    _fields_ = [("X", ctypes.c_int32),
                ("Y", ctypes.c_int32)]

user32 = ctypes.WinDLL("user32")
kernel32 = ctypes.WinDLL("kernel32")

# A/Wの区別がある場合はWを使う??
user32.MessageBoxW.restype = ctypes.c_int32
user32.MessageBoxW.argtypes = (
    ctypes.c_void_p, ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint32
)

# 関数取り出す
GetCursorPos = user32.GetCursorPos
GetCursorPos.restype = ctypes.c_bool
# 構造体のポインタ型を引数とする(参照渡し)
GetCursorPos.argtypes = (ctypes.POINTER(POINT),)

pt = POINT()
# カーソルの位置が代入される!?
GetCursorPos(pt)
print(u"x = {0}, y = {1}".format(pt.X, pt.Y))

#user32.MessageBoxW(None, u"テスト", u"タイトル", 0)

# 関数を取り出しちゃおう
GetComputerNameW = kernel32.GetComputerNameW
GetComputerNameW.restype = ctypes.c_bool
GetComputerNameW.argtypes = (
    ctypes.c_wchar_p, ctypes.POINTER(ctypes.c_uint32)
)

lenComputerName = ctypes.c_uint32()
GetComputerNameW(None, lenComputerName)
computerName = ctypes.create_unicode_buffer(lenComputerName.value)
GetComputerNameW(computerName, lenComputerName)
print(computerName.value)