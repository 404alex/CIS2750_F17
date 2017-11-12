from ctypes import *
import types


class Property(Structure):
    _field_ = [
        ('propName', c_byte * 200),
        ('propDescr', c_byte * 0)
    ]


class DateTime(Structure):
    _fields_ = [
        ('date', c_byte * 9),
        ('time', c_byte * 7),
        ('UTC', c_bool)
    ]


class Node(Structure):
    pass


Node._fields_ = [
    ('data', c_void_p),
    ('previous', POINTER(Node)),
    ('next', POINTER(Node))
]


class List(Structure):
    _fields_ = [
        ('head', POINTER(Node)),
        ('tail', POINTER(Node)),
        ('length', c_int),
        ('deleteData', CFUNCTYPE(c_void_p, c_void_p)),
        ('compare', CFUNCTYPE(c_int, c_void_p, c_void_p)),
        ('printData', CFUNCTYPE(c_char_p, c_void_p))
    ]
