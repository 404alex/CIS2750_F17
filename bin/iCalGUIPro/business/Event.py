from ctypes import *
import types
from . import Structures


class Event(Structure):
    _fields_ = [
        ('UID', c_byte * 1000),
        ('creationDateTime', POINTER(Structures.DateTime)),
        ('startDateTime', POINTER(Structures.DateTime)),
        ('properties', POINTER(Structures.List)),
        ('alarms', POINTER(Structures.List))
    ]
