# code are from the sample, which provided by Professor Nikitenko
from ctypes import *
from . import Event
from . import Structures

icalLibPath = 'bin/CalendarParserlib.so'
icalLib = CDLL(icalLibPath)

cHelpLibPath = 'bin/helplib.so'
cHelpLib = CDLL(cHelpLibPath)


class Calendar(Structure):
    _fields_ = [
        ("version", c_float),
        ("prodID", c_byte * 1000),
        ("events", POINTER(Event.Event)),
        ("properties", POINTER(Structures.Property))]


createCal = icalLib.createCalendar
createCal.argtypes = [c_char_p, POINTER(POINTER(Calendar))]
createCal.restype = c_int
deleteCal = icalLib.deleteCalendar
deleteCal.argtypes = [POINTER(Calendar)]
printCal = icalLib.printCalendar
printCal.argtypes = [POINTER(Calendar)]
printCal.restype = c_char_p
printError = icalLib.printError
printError.argtypes = [c_int]
printError.restype = c_char_p
writeCal = icalLib.writeCalendar
writeCal.argtypes = [c_char_p, POINTER(Calendar)]
writeCal.restype = c_int
valiCal = icalLib.validateCalendar
valiCal.argtypes = [POINTER(Calendar)]
valiCal.restype = c_int
calPointer = POINTER(Calendar)()
createCalGui = cHelpLib.createCalByGUI
createCalGui.argtypes = [c_float, c_char_p, c_char_p, c_char_p, c_char_p]
createCalGui.restype = POINTER(Calendar)
