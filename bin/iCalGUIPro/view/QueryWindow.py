# some code from the book: programming python 4th edition
from tkinter.messagebox import *
from tkinter.simpledialog import *
from tkinter.ttk import Treeview
from ..business import HelpMethod
from . import QueryHelpWindow


class QueryWindow(Tk):
    _dbContext = None

    def close(*args):
        result = askyesno('Close query window', 'Do you want to close?', parent=args[0])
        if result:
            args[0].destroy()

    def __init__(self, dbContext):  # , dbContext
        self._dbContext = dbContext
        Tk.__init__(self)
        HelpMethod.centerOfWindow(self, 950, 485)
        self.resizable(width=False, height=False)
        self.makeQueryInputPanel()
        self.makeLogPanel()
        self.title('Query Window')
        self.protocol('WM_DELETE_WINDOW', self.close)

    def makeQueryInputPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=0, width=950, height=150)
        panelInsideUp = Frame(panel)
        panelInsideUp.place(x=0, y=0, width=950, height=30)
        panelInsideDown = Frame(panel)
        panelInsideDown.place(x=0, y=30, width=950, height=120)
        Label(panelInsideUp, text='Execute Query:', width=13, anchor='w', font=('Helvetica', 12)).grid(row=0, column=0)
        Button(panelInsideUp, text='Submit', width=5, command=self.submitOnClick).grid(row=0, column=1)
        Button(panelInsideUp, text='Help', width=5, command=self.helpOnClick).grid(row=0, column=2)
        global _radioButtonValue
        _radioButtonValue = IntVar(self)
        Radiobutton(panelInsideDown, anchor='w', variable=_radioButtonValue, value=1,
                    text='All events sorted by start date').grid(
            row=0, column=0, sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=_radioButtonValue, value=2,
                    text='Events by organizer name: ').grid(row=1,
                                                            column=0,
                                                            sticky='w')
        self.queryByOrg = Entry(panelInsideDown)
        self.queryByOrg.grid(row=1, column=1, sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=_radioButtonValue, value=3, text='Events by location: ').grid(
            row=2, column=0,
            sticky='w')
        self.queryByLocation = Entry(panelInsideDown)
        self.queryByLocation.grid(row=2, column=1, sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=_radioButtonValue, value=4,
                    text='Contact info by organizer name: ').grid(
            row=3, column=0, sticky='w')
        self.queryByOrgCon = Entry(panelInsideDown)
        self.queryByOrgCon.grid(row=3, column=1, sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=_radioButtonValue, value=5,
                    text='Event by alarm numbers: ').grid(row=4,
                                                          column=0,
                                                          sticky='w')
        self.queryByAlarm = Entry(panelInsideDown)
        self.queryByAlarm.grid(row=4, column=1, sticky='w')
        Radiobutton(panelInsideDown, anchor='w', variable=_radioButtonValue, value=6, text='Form by yourself: ').grid(
            row=5, column=0, sticky='w')
        self.queryByUser = Entry(panelInsideDown)
        self.queryByUser.grid(row=5, column=1, sticky='w')
        self.queryByUser.insert(END, 'SELECT ')
        _radioButtonValue.set(1)

    def cleanOnClick(*args):
        _globelLogInfoText.config(state=NORMAL)
        _globelLogInfoText.delete('1.0', END)
        _globelLogInfoText.config(state=DISABLED)

    def submitOnClick(*args):
        if _radioButtonValue.get() == 1:
            args[0].logInfoText(args[0]._dbContext.allEventOrderByDate())
        elif _radioButtonValue.get() == 2:
            if args[0].queryByOrg.get() == "":
                showerror('Error', 'Please input something in textbox first')
                return
            args[0].logInfoText(args[0]._dbContext.getEventByOrgName(args[0].queryByOrg.get()))
        elif _radioButtonValue.get() == 3:
            if args[0].queryByLocation.get() == "":
                showerror('Error', 'Please input something in textbox first')
                return
            args[0].logInfoText(args[0]._dbContext.getEventByLocation(args[0].queryByLocation.get()))
        elif _radioButtonValue.get() == 4:
            if args[0].queryByOrgCon.get() == "":
                showerror('Error', 'Please input something in textbox first')
                return
            args[0].logInfoText(args[0]._dbContext.getContactInfoByOrg(args[0].queryByOrgCon.get()))
        elif _radioButtonValue.get() == 5:
            if args[0].queryByAlarm.get() == "":
                showerror('Error', 'Please input something in textbox first')
                return
            args[0].logInfoText(args[0]._dbContext.getEventByAlarm(args[0].queryByAlarm.get()))
        elif _radioButtonValue.get() == 6:
            if args[0].queryByUser.get() == "":
                showerror('Error', 'Please input something in textbox first')
                return
            args[0].logInfoText(args[0]._dbContext.getEventByUser(args[0].queryByUser.get()))

    def helpOnClick(*args):
        string = args[0]._dbContext.helpButton()
        QueryHelpWindow.dialog(string)

    def makeLogPanel(self):
        panel = Frame(self)
        panel.place(x=0, y=155, width=950, height=330)
        panelInsideUp = Frame(panel)
        panelInsideUp.place(x=0, y=0, width=950, height=30)
        panelInsideDown = Frame(panel)
        panelInsideDown.place(x=0, y=30, width=950, height=300)
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
