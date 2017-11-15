#some code from the book: programming python 4th edition
from tkinter import *
from GUI.business import HelpMethod


def dialog():
    win = Toplevel()
    HelpMethod.centerOfWindow(win, 240, 170)
    win.resizable(width=False, height=False)
    win.focus_set()
    win.grab_set()
    win.title('About iCalGUI')
    Label(win, text='iCalGUI', fg='green', font=('Helvetica', 20)).grid(row=0, column=0, padx=10, pady=10, columnspan=2)
    Label(win, text='Creator: ').grid(row=1, column=0, sticky=E)
    Label(win, text='Chenxingyu Chen').grid(row=1, column=1, sticky=W)
    Label(win, text='Version:').grid(row=2, column=0, sticky=E)
    Label(win, text='1.0').grid(row=2, column=1, sticky=W)
    Label(win, text='  iCalendar Version:').grid(row=3, column=0, sticky=E)
    Label(win, text='2.0').grid(row=3, column=1, sticky=W)
    Label(win, text='Credit:').grid(row=4, column=0, sticky=E)
    Label(win, text='None').grid(row=4, column=1, sticky=W)
    Button(win, text='Close', command=win.destroy).grid(row=5, column=0, columnspan=2)
