# -*- coding: utf-8 -*-
# ctypes の練習
import numpy as np
import sys
import ctypes

MAX_PATH = 260
INVALID_HANDLE_VALUE = -1

# 構造体を定義?
class POINT(ctypes.Structure):
    # ただ変数をひとつ作っておく
    # _fields_ という名前も重要?
    _fields_ = [("X", ctypes.c_int32),
                ("Y", ctypes.c_int32)]

class FILETIME(ctypes.Structure):
    _fields_ = [("dwLowDateTime", ctypes.c_uint32),
                ("dwHighDateTime", ctypes.c_uint32)]

# 上で定義した型を含んだ型
# 何が何だかさっぱり
class WIN32_FINE_DATAW(ctypes.Structure):
    _fields_ = [
        ("dwFileAttributes", ctypes.c_uint32),
        ("ftCreationTime", FILETIME),
        ("ftLastAccessTime", FILETIME),
        ("ftLastWriteTime", FILETIME),
        ("nFileSizeHigh", ctypes.c_uint32),
        ("nFileSizeLow", ctypes.c_uint32),
        ("dwReserved0", ctypes.c_wchar),
        ("dwReserved1", ctypes.c_uint32),
        ("cFileName", ctypes.c_wchar * MAX_PATH),
        ("cAlternateFileName", ctypes.c_wchar * 14),
        ("dwFileType", ctypes.c_uint32),
        ("dwCreatorType", ctypes.c_uint32),
        ("wFinderFlags", ctypes.c_uint16)
    ]

# ファイル読み込む?
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

# なんか関数取り出す
FindFirstFileW = kernel32.FindFirstFileW
FindNextFileW = kernel32.FindNextFileW
# 付け足し忘れてた関数!
FindClose = kernel32.FindClose

FindFirstFileW.restype = ctypes.c_void_p
# 謎の型のポインタを与える
FindFirstFileW.argtypes = {
    ctypes.c_wchar_p, ctypes.POINTER(WIN32_FINE_DATAW)
}
FindNextFileW.restype = ctypes.c_bool
FindNextFileW.argtypes = (
    ctypes.c_void_p, ctypes.POINTER(WIN32_FINE_DATAW)
)
FindClose.restype = ctypes.c_bool
FindClose.argtypes = (ctypes.c_void_p,)

pattern = "C:\Windows\*.exe"
# インスタンス作成
findData = WIN32_FINE_DATAW()
hfind = FindFirstFileW(pattern, findData)
if hfind != INVALID_HANDLE_VALUE:
    while True:
        print(findData.cFileName)
        if not FindNextFileW(hfind, findData):
            break
    # ?
    FindClose(hfind)