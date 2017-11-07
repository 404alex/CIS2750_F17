from tkinter import *
from GUI.business import HelpMethod
from tkinter.messagebox import *


class CreateCalendarWindow:

    def cancel(*args):
        result = askyesno('Cancel Action', 'Do you want to cancel?', parent=args[0].top)
        if result:
            args[0].top.destroy()

    def __init__(self, parent):
        win = self.top = Toplevel(parent)
        HelpMethod.centerOfWindow(win, 560, 480)
        win.resizable(width=False, height=False)
        win.focus_set()
        win.grab_set()
        win.protocol("WM_DELETE_WINDOW", self.cancel)
        win.title('Create New Calendar')
        Label(win, text='iCalGUI', fg='green', font=('Helvetica', 20)).grid(row=0, column=0, padx=10, pady=10,
                                                                            columnspan=2)
        Label(win, text='Creator: ').grid(row=1, column=0, sticky=E)
        Label(win, text='Chenxingyu Chen').grid(row=1, column=1, sticky=W)
        Label(win, text='Version:').grid(row=2, column=0, sticky=E)
        Label(win, text='1.0').grid(row=2, column=1, sticky=W)
        Label(win, text='  iCalendar Version:').grid(row=3, column=0, sticky=E)
        Label(win, text='2.0').grid(row=3, column=1, sticky=W)
        Label(win, text='Credit:').grid(row=4, column=0, sticky=E)
        Label(win, text='None').grid(row=4, column=1, sticky=W)
        Button(win, text='Ok', command=self.cancel).grid(row=5, column=0, columnspan=2)
        Button(win, text='Cancel', command=self.cancel).grid(row=5, column=0, columnspan=2)
