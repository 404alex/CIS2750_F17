from tkinter import *
from ..business import HelpMethod


def dialog(string):
    win = Toplevel()
    HelpMethod.centerOfWindow(win, 920, 245)
    win.resizable(width=False, height=False)
    win.focus_set()
    win.grab_set()
    win.title('Query Info')
    text = Text(win, relief=SUNKEN,heigh=15,width=130)
    text.insert(INSERT, string)
    text.grid(row=0)
    text.config(state=DISABLED)
    Button(win, text='Close', command=win.destroy).grid(row=1)
