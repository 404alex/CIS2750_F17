# some code from the book: programming python 4th edition
from tkinter.messagebox import *
from tkinter.simpledialog import *
from tkinter.ttk import Treeview
from bin.iCalGUIPro.business import HelpMethod


class QueryWindow(Tk):
    _dbContext = None

    def close(*args):
        result = askyesno('Close query window', 'Do you want to close?', parent=args[0])
        if result:
            args[0].top.destroy()

    def __init__(self):  # , dbContext
        # self._dbContext = dbContext
        Tk.__init__(self)
        HelpMethod.centerOfWindow(self, 580, 485)
        self.resizable(width=False, height=False)
        self.makeQueryInputPanel()
        self.makeLogPanel()
        self.protocol('WM_DELETE_WINDOW', self.close)

    def makeQueryInputPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=0, width=580, height=150)
        panelInsideUp = Frame(panel)
        panelInsideUp.place(x=0, y=0, width=580, height=30)
        panelInsideDown = Frame(panel)
        panelInsideDown.place(x=0, y=30, width=580, height=120)
        Label(panelInsideUp, text='Execute Query:', width=13, anchor='w', font=('Helvetica', 12)).grid(row=0, column=0)
        Button(panelInsideUp, text='Submit', width=5, command=self.submitOnClick).grid(row=0, column=1)
        Button(panelInsideUp, text='Help', width=5, command=self.helpOnClick).grid(row=0, column=2)
        global radioButtonValue
        radioButtonValue = IntVar()
        radioButtonValue.set(1)
        Radiobutton(panelInsideDown, anchor='w', variable=radioButtonValue, value=1, text='All events sorted by start date').grid(
            row=0, column=0, sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=radioButtonValue, value=2, text='Events by organizer name: ').grid(row=1,
                                                                                                              column=0,
                                                                                                              sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=radioButtonValue, value=3, text='Events by location: ').grid(row=2, column=0,
                                                                                                        sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=radioButtonValue, value=4, text='Contact info by organizer name: ').grid(
            row=3, column=0, sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=radioButtonValue, value=5, text='Event by alarm numbers: ').grid(row=4,
                                                                                                            column=0,
                                                                                                            sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=radioButtonValue, value=6, text='Form by yourself: ').grid(row=5, column=0, sticky='w')
        queryByUser = Entry(panelInsideDown)
        queryByUser.grid(row=5, column=1, sticky='w')
        queryByUser.insert(END,'SELECT ')

    def cleanOnClick(*args):
        _globelLogInfoText.config(state=NORMAL)
        _globelLogInfoText.delete('1.0', END)
        _globelLogInfoText.config(state=DISABLED)

    def submitOnClick(*args):
        args[0].logInfoText('submit')

    def helpOnClick(*args):
        args[0].logInfoText('help')

    def makeLogPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=155, width=580, height=330)
        panelInsideUp = Frame(panel)
        panelInsideUp.place(x=0, y=0, width=580, height=30)
        panelInsideDown = Frame(panel)
        panelInsideDown.place(x=0, y=30, width=580, height=300)
        label = Label(panelInsideUp, text='Query Result:', width=12, anchor='w', font=('Helvetica', 12)).grid(row=0,
                                                                                                              column=0)
        button = Button(panelInsideUp, text='Clear', width=5, command=self.cleanOnClick).grid(row=0, column=1)
        scrollBar = Scrollbar(panelInsideDown)
        scrollBar.pack(side=RIGHT, fill=Y)
        global _globelLogInfoText
        _globelLogInfoText = Text(panelInsideDown, relief=SUNKEN, state=DISABLED)
        scrollBar.config(command=_globelLogInfoText.yview)
        _globelLogInfoText.config(yscrollcommand=scrollBar.set)
        scrollBar.pack(side=RIGHT, fill=Y)
        _globelLogInfoText.pack(side=LEFT, expand=YES, fill=BOTH)

    def logInfoText(self, text):
        _globelLogInfoText.config(state=NORMAL)
        _globelLogInfoText.insert(INSERT, text)
        _globelLogInfoText.config(state=DISABLED)
        _globelLogInfoText.see(END)


# todo test need delete
root = QueryWindow()
root.mainloop()
# todo end of need delete
