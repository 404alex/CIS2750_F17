from tkinter import *
from tkinter.messagebox import *
import sys
from ctypes import *


def centerOfWindow(window, w, h):
    width = window.winfo_screenwidth()
    height = window.winfo_screenheight()
    middle_x = width / 2 - w / 2
    middle_y = height / 2 - h / 2
    window.geometry('%dx%d+%d+%d' % (w, h, middle_x, middle_y))


def newCalParameterArray(dict):
    maxLength = 0
    for key, value in dict.items():
        if maxLength < len(value):
            maxLength = len(value)

    maxLength += 5
    if len(dict) != 5:
        return None
    array = ((c_char * maxLength) * len(dict))()
    array[0].value = dict['creationDateTime'].encode('UTF-8')
    array[1].value = dict['startDateTime'].encode('UTF-8')
    array[2].value = dict['prodID'].encode('UTF-8')
    array[3].value = dict['version'].encode('UTF-8')
    array[4].value = dict['UID'].encode('UTF-8')
    return array
