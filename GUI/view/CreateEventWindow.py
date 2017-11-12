from tkinter import *
from GUI.business import HelpMethod
from tkinter.messagebox import *
import uuid


class CreateEventWindow:
    isCancel = False

    def cancel(*args):
        result = askyesno('Cancel Action', 'Do you want to cancel?', parent=args[0].top)
        if result:
            args[0].isCancel = True
            args[0].top.destroy()

    def __init__(self, parent):
        win = self.top = Toplevel(parent)
        HelpMethod.centerOfWindow(win, 375, 375)
        win.resizable(width=False, height=False)
        win.focus_set()
        win.grab_set()
        win.protocol("WM_DELETE_WINDOW", self.cancel)
        win.title('Create Event')
        win.grid_columnconfigure(1, weight=1)
        Label(win, text='UUID: ').grid(row=0, column=0)
        Text(win, name='_textUUID', bg='#e2e2e9', height=1, width=37).grid(row=0, column=1, columnspan=2)
        calUUID = str(uuid.uuid4())
        win.nametowidget('_textUUID').insert('1.0', calUUID)
        win.nametowidget('_textUUID').config(state=DISABLED)
        Button(win, text='Ok', command=self.cancel, width=5).grid(row=5, column=1, sticky=E)
        Button(win, text='Cancel', command=self.cancel).grid(row=5, column=2, sticky=W)
