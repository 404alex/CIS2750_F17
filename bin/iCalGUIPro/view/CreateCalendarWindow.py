#some code from the book: programming python 4th edition

from tkinter import *
from ..business import HelpMethod
from tkinter.messagebox import *
from . import CreateEventWindow


class CreateCalendarWindow:
    calArgs = {}

    def cancel(*args):
        result = askyesno('Cancel Action', 'Do you want to cancel?', parent=args[0].top)
        if result:
            args[0].calArgs = {}
            args[0].top.destroy()

    def ok(*args):
        eventWin = CreateEventWindow.CreateEventWindow(args[1])
        args[1].wait_window(eventWin.top)
        if eventWin.isCancel:
            showerror('Error', 'You have to create a new event!')
        else:
            args[0].calArgs = {**eventWin.eventArgs, **args[0].calArgs}
            args[0].top.destroy()

    def __init__(self, parent):
        win = self.top = Toplevel(parent)
        HelpMethod.centerOfWindow(win, 375, 69)
        win.resizable(width=False, height=False)
        win.focus_set()
        win.grab_set()
        win.protocol("WM_DELETE_WINDOW", self.cancel)
        win.title('Create New Calendar')
        win.grid_columnconfigure(1, weight=1)
        # Label(win, text='UUID: ').grid(row=0, column=0)
        # Text(win, name='_textUUID', bg='#e2e2e9', height=1, width=37).grid(row=0, column=1, columnspan=2)
        Label(win, text='Version: ').grid(row=1, column=0)
        Text(win, name='_textVersion', bg='#e2e2e9', height=1, width=37).grid(row=1, column=1, columnspan=2)
        Label(win, text='Product Identity: ').grid(row=2, column=0)
        Text(win, name='_prodid', bg='#e2e2e9', height=1, width=37).grid(row=2, column=1, columnspan=2)

        # calUUID = str(uuid.uuid4())
        # win.nametowidget('_textUUID').insert('1.0', calUUID)
        # win.nametowidget('_textUUID').config(state=DISABLED)
        win.nametowidget('_textVersion').insert('1.0', '2.0')
        win.nametowidget('_textVersion').config(state=DISABLED)
        win.nametowidget('_prodid').insert('1.0', '-//iCalGUI//F17CIS2750//CN')
        win.nametowidget('_prodid').config(state=DISABLED)
        self.calArgs['version'] = '2.0'
        self.calArgs['prodID'] = '-//iCalGUI//F17CIS2750//CN'

        Button(win, text='Create Event', command=lambda: self.ok(win), width=10).grid(row=5, column=1, sticky=E)
        Button(win, text='Cancel', command=self.cancel).grid(row=5, column=2, sticky=W)
