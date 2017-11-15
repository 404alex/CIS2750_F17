from ctypes import *
import types
from . import Structures


class Alarm:
    _fields_ = [
        ('action', c_byte * 200),
        ('trigger', c_char_p),
        ('properties', POINTER(Structures.List))
    ]
