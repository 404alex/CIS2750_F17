# code are from the sample, which provided by Professor Nikitenko
from ctypes import *
from . import Event
from . import Structures

icalLibPath = 'bin/parserlib.so'
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
createCalGui.argtypes = [c_float, c_char_p, c_char_p, c_char_p, c_char_p, POINTER(POINTER(Calendar))]
createCalGui.restype = None
getRowResult = cHelpLib.getRowInfo
getRowResult.argtypes = [POINTER(Calendar)]
getRowResult.restype = POINTER(c_char_p)
printEveOp = cHelpLib.printEventForUI
printEveOp.argtypes = [c_int, POINTER(Calendar)]
printEveOp.restype = c_char_p
printEveAl = cHelpLib.printAlarmForUI
printEveAl.argtypes = [c_int, POINTER(Calendar)]
printEveAl.restype = c_char_p
createEve = cHelpLib.createEvent
createEve.argtypes = [c_char_p, c_char_p, c_char_p, POINTER(Calendar)]
createEve.restype = None
freeString = cHelpLib.freeString
freeString.argtypes = [c_char_p]
freeString.restype = None
