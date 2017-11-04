from tkinter import *
from tkinter.messagebox import *
import sys


def centerOfWindow(window, w, h):
    width = window.winfo_screenwidth()
    height = window.winfo_screenheight()
    middle_x = width / 2 - w / 2
    middle_y = height / 2 - h / 2
    window.geometry('%dx%d+%d+%d' % (w, h, middle_x, middle_y))
